#include "vm/processor.hpp"

#include <utility>
#include <string>
#include <type_traits>
#include <cstdint>

namespace perseus
{
  namespace detail
  {
    processor::processor( code_segment&& code, const instruction_pointer::value_type start_address )
      : _code( std::move( code ) )
    {
      _active_coroutine_stack.push_back( &_coroutine_manager.new_coroutine( _code, start_address ) );
    }

    processor::execution_result processor::continue_execution( stack&& parameters )
    {
      if( _active_coroutine_stack.empty() )
      {
        throw execution_finished( "Trying to continue_execution() of a finished program!" );
      }
      coroutine* active_coroutine = _active_coroutine_stack.back();
      active_coroutine->stack.append( parameters );
      parameters.clear();
      while( true )
      {
        instruction_pointer& ip = active_coroutine->instruction_pointer;
        const opcode instruction = ip.read< opcode >();
        switch( instruction )
        {
        case opcode::no_operation:
          break;
        case opcode::exit:
        {
          stack::size_type result_size = ip.read< std::uint32_t >();
          stack result_stack = active_coroutine->stack.split( result_size );
          _active_coroutine_stack.clear();
          _coroutine_manager.clear();
          // workaround for MSVC14RC bug where destructors would not get called - create a temporary object and move that.
          execution_result result { true, std::move( result_stack ) };
          return result;
        }
        case opcode::exit_returning_everything:
        {
          stack result_stack = std::move( active_coroutine->stack );
          _active_coroutine_stack.clear();
          _coroutine_manager.clear();
          // workaround for MSVC14RC bug where destructors would not get called - create a temporary object and move that.
          execution_result result{ true, std::move( result_stack ) };
          return result;
        }
        default:
          throw invalid_opcode( "Invalid opcode " + std::to_string( static_cast< std::underlying_type_t< opcode > >( instruction ) ) );
        }
        // TODO
      }
    }
  }
}
