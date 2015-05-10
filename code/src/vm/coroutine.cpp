#include "vm/coroutine.hpp"

namespace perseus
{
  namespace detail
  {
    coroutine::coroutine( const identifier index, const code_segment& code, const instruction_pointer::value_type start_address )
      : index( index )
      , instruction_pointer( code, start_address )
    {
    }
  }
}
