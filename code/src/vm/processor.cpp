#include "vm/processor.hpp"

#include <utility>
#include <string>
#include <type_traits>

namespace perseus
{
  namespace detail
  {
    processor::processor( code_segment&& code )
      : _code( std::move( code ) )
      , _instruction_pointer( _code )
    {
    }

    void processor::execute()
    {
      while( true )
      {
        const opcode instruction = _instruction_pointer.read< opcode >();
        switch( instruction )
        {
        case opcode::no_operation:
          break;
        case opcode::exit:
          return;
        default:
          throw invalid_opcode( "Invalid opcode " + std::to_string( static_cast< std::underlying_type_t< opcode > >( instruction ) ) );
        }
        // TODO
      }
    }
  }
}
