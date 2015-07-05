#include "compiler/compile.hpp"
#include "compiler/iterators.hpp"

namespace perseus
{
  void compile( std::istream& source_stream, const std::string& filename )
  {
    detail::enhanced_istream_iterator begin, end;
    std::tie( begin, end ) = detail::enhanced_iterators( source_stream );
  }
}
