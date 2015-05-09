#include "shared/opcodes.hpp"

#include <type_traits>

namespace perseus
{
  namespace detail
  {
    std::ostream& operator<<( std::ostream& stream, const opcode code )
    {
      stream << "opcode(" << static_cast< std::underlying_type_t< opcode > >( code ) << ')';
      return stream;
    }
  }
}
