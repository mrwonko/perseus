#pragma once

#include <boost/spirit/home/lex/lexer/lexer.hpp>

#include <ostream>

namespace perseus
{
  namespace detail
  {
    namespace token_id
    {
      enum token_id
      {
        whitespace = boost::spirit::lex::min_token_id,
        comment,
        string,
        identifier,
        any
      };

      std::ostream& operator<<( std::ostream& os, token_id token );
    }
  }
}