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

        // constants
        string,
        decimal_integer,
        hexadecimal_integer,
        binary_integer,

        identifier,
        operator_identifier,

        // keywords; using a _ suffix for conistency, even for those that are not c++ keywords
        if_,
        else_,
        while_,
        return_,
        function_,
        let_,

        colon,
        semicolon,
        dot,
        comma,
        equals,
        backtick, // `
        arrow_right, // ->

        // ()
        paren_open,
        paren_close,
        // {}
        brace_open,
        brace_close,
        // []
        square_bracket_open,
        square_bracket_close,

        any
      };

      std::ostream& operator<<( std::ostream& os, token_id token );
    }
  }
}