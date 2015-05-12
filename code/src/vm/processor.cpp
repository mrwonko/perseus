#include "vm/processor.hpp"

#include <utility>
#include <string>
#include <type_traits>
#include <cstdint>

namespace perseus
{
  namespace detail
  {
    processor::processor( code_segment&& code )
      : _code( std::move( code ) )
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
        default:
          throw invalid_opcode( "Invalid opcode " + std::to_string( static_cast< std::underlying_type_t< opcode > >( instruction ) ) );
        }
      }
    }
  }
}
