#pragma once

#include "ast.hpp"

namespace perseus
{
  namespace detail
  {
    struct parsed_string_literal : ast::string_literal
    {
      parsed_string_literal() = default;
      parsed_string_literal( const enhanced_istream_iterator& begin, const enhanced_istream_iterator& end );
    };
  }
}
