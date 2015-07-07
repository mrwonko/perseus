#pragma once

#include "token_definitions.hpp"

#include <string>

namespace perseus
{
  namespace detail
  {
    struct parsed_string_literal : std::u32string
    {
      parsed_string_literal() = default;
      parsed_string_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );
    };
  }
}
