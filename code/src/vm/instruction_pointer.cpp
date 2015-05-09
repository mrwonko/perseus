#include "vm/instruction_pointer.hpp"

namespace perseus
{
  namespace detail
  {
    instruction_pointer::instruction_pointer( const code_segment& code, const code_segment::size_type offset )
      : _code( &code )
      , _offset( offset )
    {
    }
  }
}
