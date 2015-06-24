#include "vm/processor.hpp"

#include <utility>
#include <string>
#include <type_traits>
#include <cstdint>
#include <algorithm>
#include <iterator>

namespace perseus
{
  namespace detail
  {
    processor::processor( code_segment&& code, std::vector< syscall >&& syscalls )
      : _code( std::move( code ) )
      , _syscalls( std::move( syscalls ) )
    {
    }

    stack processor::execute( const instruction_pointer::value_type start_address, stack&& parameters )
    {
      std::vector< coroutine* > active_coroutines{ &_coroutine_manager.new_coroutine( _code, start_address, std::move( parameters ) ) };
      while( true )
      {
        coroutine& co = *active_coroutines.back();
        instruction_pointer& ip = co.instruction_pointer;
        const opcode instruction = ip.read< opcode >();
        switch( instruction )
        {
        case opcode::no_operation:
          break;
        case opcode::exit:
        {
          if( active_coroutines.size() > 1 )
          {
            throw exit_in_coroutine( "opcode::exit in coroutine!" );
          }
          stack result = std::move( co.stack );
          co.current_state = coroutine::state::dead;
          _coroutine_manager.delete_coroutine( co.index );
          return result;
        }
        case opcode::syscall:
        {
          const std::uint32_t index = ip.read< std::uint32_t >();
          if( index >= _syscalls.size() )
          {
            throw invalid_syscall( "Invalid syscall!" );
          }
          _syscalls[ index ]( co.stack );
          break;
        }

        //    Coroutines
        case opcode::absolute_coroutine:
        case opcode::indirect_coroutine:
        {
          const coroutine::identifier id = _coroutine_manager.new_coroutine(
            _code,
            instruction == opcode::absolute_coroutine ? ip.read< instruction_pointer::value_type >() : co.stack.pop< instruction_pointer::value_type >()
          ).index;
          co.stack.push< coroutine::identifier >( id );
          break;
        }
        case opcode::resume_coroutine:
        case opcode::resume_pushing_everything:
        {
          coroutine& co_to_resume = _coroutine_manager.get_coroutine( co.stack.pop< coroutine::identifier >() );
          if( co_to_resume.current_state == coroutine::state::live )
          {
            throw resuming_live_coroutine( "Trying to resume a live coroutine!" );
          }
          if( co_to_resume.current_state == coroutine::state::dead )
          {
            throw resuming_dead_coroutine( "Trying to resume a dead coroutine!" );
          }
          co_to_resume.stack.append( instruction == opcode::resume_coroutine ? co.stack.split( ip.read< std::uint32_t >() ) : std::move( co.stack ) );
          co_to_resume.current_state = coroutine::state::live;
          active_coroutines.push_back( &co_to_resume );
          break;
        }
        case opcode::coroutine_state:
          co.stack.push< coroutine::state >( _coroutine_manager.get_coroutine( co.stack.pop < coroutine::identifier >() ).current_state );
          break;
        case opcode::delete_coroutine:
          _coroutine_manager.delete_coroutine( co.stack.pop< coroutine::identifier >() );
          break;
        case opcode::coroutine_return:
        case opcode::yield:
        {
          // is this the root coroutine? can't leave that one without exit.
          if( active_coroutines.size() == 1 )
          {
            throw no_coroutine( std::string( "Trying to " ) + ( instruction == opcode::yield ? "yield" : "return" ) + " from last coroutine!" );
          }
          co.current_state = instruction == opcode::yield ? coroutine::state::suspended : coroutine::state::dead;
          active_coroutines.pop_back();
          coroutine& previous_co = *active_coroutines.back();
          previous_co.stack.append( co.stack.split( co.instruction_pointer.read< std::uint32_t >() ) );
          break;
        }

        //    Push/Pop
        case opcode::push_8:
          co.stack.push< char >( ip.read< char >() );
          break;
        case opcode::push_32:
          co.stack.push< std::int32_t >( ip.read< std::int32_t >() );
          break;
        case opcode::pop:
          co.stack.discard( ip.read< std::uint32_t >() );
          break;

        //    Load/Store
        case opcode::absolute_load_current_stack:
        case opcode::absolute_load_stack:
        case opcode::relative_load_stack:
        {
          const std::uint32_t size = ip.read< std::uint32_t >();
          // FIXME: does not catch underflow errors
          // note: co.stack.size() would have to be from_co.stack.size(), but in the case of relative_load_stack from_co = co
          const std::uint32_t address = instruction == opcode::relative_load_stack ? co.stack.pop< std::int32_t >() + co.stack.size() : co.stack.pop< std::uint32_t >();
          const coroutine& from_co = instruction == opcode::absolute_load_stack ? _coroutine_manager.get_coroutine( co.stack.pop< std::uint32_t >() ) : co;
          if( address + size > from_co.stack.size() )
          {
            throw stack_segmentation_fault( "stack segmentation fault: read above top of stack" );
          }
          // ensure there are no iterator-invalidating reallocations during copying
          co.stack.reserve( co.stack.size() + size );
          std::copy( from_co.stack.begin() + address, from_co.stack.begin() + address + size, std::back_inserter( co.stack ) );
          break;
        }
        case opcode::absolute_store_current_stack:
        case opcode::absolute_store_stack:
        case opcode::relative_store_stack:
        {
          const std::uint32_t size = ip.read< std::uint32_t >();
          // FIXME: does not catch underflow errors
          // as above: co.stack.size() should be to_co.stack.size(), but for relative_store_stack to_co = co
          const std::uint32_t address = instruction == opcode::relative_store_stack ? co.stack.pop< std::int32_t >() + co.stack.size() : co.stack.pop< std::uint32_t >();
          coroutine& to_co = instruction == opcode::absolute_store_stack ? _coroutine_manager.get_coroutine( co.stack.pop< std::uint32_t >() ) : co;
          if( address + size > to_co.stack.size() )
          {
            throw stack_segmentation_fault( "stack segmentation fault: write above top of stack" );
          }
          std::copy( co.stack.end() - size, co.stack.end(), to_co.stack.begin() + address );
          co.stack.discard( size );
          break;
        }

        //    Jumps/Calls
        case opcode::absolute_jump:
          ip = ip.read< std::uint32_t >();
          break;
        case opcode::relative_jump:
          ip += ip.read< std::int32_t >();
          break;
        case opcode::relative_jump_if_false:
        {
          std::int32_t offset = ip.read< std::int32_t >();
          if( !co.stack.pop< std::uint8_t >() )
          {
            ip += offset;
          }
          break;
        }
        case opcode::indirect_jump:
          ip = co.stack.pop< std::uint32_t >();
          break;
        case opcode::call:
        {
          const std::uint32_t target_address = ip.read< std::uint32_t >();
          co.stack.push< std::uint32_t >( ip.value() );
          ip = target_address;
          break;
        }
        case opcode::indirect_call:
        {
          const std::uint32_t target_address = co.stack.pop< std::uint32_t >();
          co.stack.push< std::uint32_t >( ip.value() );
          ip = target_address;
          break;
        }
        case opcode::return_:
        {
          const std::uint32_t return_address = co.stack.pop< std::uint32_t >();
          // discard parameters
          co.stack.discard( ip.read< std::uint32_t >() );
          ip = return_address;
          break;
        }

        //    Boolean operations
        case opcode::and_b:
        {
          const std::uint8_t op2 = co.stack.pop< std::uint8_t >(), op1 = co.stack.pop< std::uint8_t >();
          co.stack.push< std::uint8_t >( op1 && op2 );
          break;
        }
        case opcode::or_b:
        {
          const std::uint8_t op2 = co.stack.pop< std::uint8_t >(), op1 = co.stack.pop< std::uint8_t >();
          co.stack.push< std::uint8_t >( op1 || op2 );
          break;
        }
        case opcode::equals_b:
        {
          const std::uint8_t op2 = co.stack.pop< std::uint8_t >(), op1 = co.stack.pop< std::uint8_t >();
          co.stack.push< std::uint8_t >( op1 == op2 );
          break;
        }
        case opcode::not_equals_b:
        {
          const std::uint8_t op2 = co.stack.pop< std::uint8_t >(), op1 = co.stack.pop< std::uint8_t >();
          co.stack.push< std::uint8_t >( op1 != op2 );
          break;
        }
        case opcode::negate_b:
          co.stack.push< std::uint8_t >( !co.stack.pop< std::uint8_t >() );
          break;

        //    32 bit integer operations
        case opcode::add_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::int32_t >( op1 + op2 );
          break;
        }
        case opcode::subtract_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::int32_t >( op1 - op2 );
          break;
        }
        case opcode::multiply_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::int32_t >( op1 * op2 );
          break;
        }
        case opcode::divide_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          if( op2 == 0 )
          {
            throw divide_by_zero( "divide by zero" );
          }
          co.stack.push< std::int32_t >( op1 / op2 );
          break;
        }
        case opcode::equals_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::uint8_t >( op1 == op2 );
          break;
        }
        case opcode::not_equals_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::uint8_t >( op1 != op2 );
          break;
        }
        case opcode::less_than_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::uint8_t >( op1 < op2 );
          break;
        }
        case opcode::less_than_or_equals_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          co.stack.push< std::uint8_t >( op1 <= op2 );
          break;
        }
        case opcode::negate_i32:
          co.stack.push< std::int32_t >( -co.stack.pop< std::int32_t >() );
          break;
        case opcode::modulo_i32:
        {
          const std::int32_t op2 = co.stack.pop< std::int32_t >(), op1 = co.stack.pop< std::int32_t >();
          if( op2 == 0 )
          {
            throw divide_by_zero( "divide by zero" );
          }
          co.stack.push< std::int32_t >( op1 % op2 );
          break;
        }

        default:
          throw invalid_opcode( "Invalid opcode " + std::to_string( static_cast< std::underlying_type_t< opcode > >( instruction ) ) );
        }
      }
    }
  }
}
