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
        whitespace = 1,
        comment,

        // constants
        string,
        decimal_integer,
        hexadecimal_integer,
        binary_integer,

        identifier,
        operator_identifier,

        //    keywords; using a _ suffix for conistency, even for those that are not c++ keywords
        // control flow
        if_,
        else_,
        while_,
        return_,
        // values
        true_,
        false_,
        // other
        let_,
        function_,
        mutable_,
        impure_,

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