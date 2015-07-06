#pragma once

#include <boost/spirit/home/lex/lexer/lexer.hpp>

#include <cstdint>
#include <string>

#include "token_ids.hpp"

namespace perseus
{
  namespace detail
  {
    /**
    @brief Definition of the various tokens

    The first phase of perseus compilation is tokenization, or lexical analysis. The char stream is split into tokens according to regular expressions; those tokens are then parsed according to the grammar.
    */
    template< typename lexer >
    class token_definitions : public boost::spirit::lex::lexer< lexer >
    {
    public:
      /// constructor (contains definitions)
      token_definitions()
      {
        self.add
          ( R"(\s+)", token_id::whitespace )
          ( R"(\/\/[^\n]*|\/\*([^\*]|\*[^\/])*\*\/)", token_id::comment )
          ( R"(\"([^\n\"\\]|\\[^\n])*\")", token_id::string )
          ( R"(\w+)", token_id::identifier ) // alphanumeric and underscore
          ;
      }
    };
  }
}
