#include "vm/coroutine.hpp"

#include <utility>

namespace perseus
{
  namespace detail
  {
    coroutine::coroutine( const identifier index, const code_segment& code, const instruction_pointer::value_type start_address, perseus::stack&& initial_stack )
      : index( index )
      , stack( std::move( initial_stack ) )
      , instruction_pointer( code, start_address )
    {
    }
  }
}
