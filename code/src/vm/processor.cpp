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
          if( active_coroutines.size() > 1 )
          {
            throw exit_in_coroutine( "opcode::exit in coroutine!" );
          }
          return std::move( co.stack );
        default:
          throw invalid_opcode( "Invalid opcode " + std::to_string( static_cast< std::underlying_type_t< opcode > >( instruction ) ) );
        }
        // TODO
      }
    }
  }
}
