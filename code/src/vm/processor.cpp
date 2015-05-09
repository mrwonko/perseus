#include "vm/processor.hpp"
#include <utility>

namespace perseus
{
  namespace detail
  {
    processor::processor( code_segment&& code )
      : _code( std::move( code ) )
    {
    }

    void processor::execute()
    {
      code_segment::const_pointer instruction_pointer = _code.data();
      while( true )
      {
        // TODO
      }
    }
  }
}
