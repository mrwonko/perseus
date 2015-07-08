#pragma once

#include <boost/spirit/home/lex/lexer/lexer.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/token.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/lexer.hpp>

#include "token_ids.hpp"
#include "iterators.hpp"

namespace perseus
{
  namespace detail
  {
    typedef boost::spirit::lex::lexertl::token<
      enhanced_istream_iterator,
      boost::mpl::vector<>, // don't do automatic attribute conversion, just supply the plain iterator ranges
      boost::mpl::false_, // not interested in lexer states
      perseus::detail::token_id::token_id // token id type
    > token;
    typedef boost::spirit::lex::lexertl::lexer< token > lexer;

    /**
    @brief Definition of the various tokens

    The first phase of perseus compilation is tokenization, or lexical analysis. The char stream is split into tokens according to regular expressions; those tokens are then parsed according to the grammar.
    */
    class token_definitions : public boost::spirit::lex::lexer< lexer >
    {
    public:
      /// constructor (contains definitions)
      token_definitions()
      {
        self.add_pattern
          ( "HEX", "[0-9a-zA-Z]" )
          ;

        self.add
          ( R"(\xEF\xBB\xBF)", token_id::byte_order_mark )
          ( R"(\s+)", token_id::whitespace )
          ( R"(\/\/[^\n]*|\/\*([^\*]|\*[^\/])*\*\/)", token_id::comment )

          // literals
          ( R"(\"([^\n\"\\]|\\[\"nrt\\])*\")", token_id::string )
          ( R"(\d('?\d+)*)", token_id::decimal_integer )
          ( R"(0[xX]{HEX}('?{HEX}+)*)", token_id::hexadecimal_integer )
          ( R"(0[bB][01]('?[01]+)*)", token_id::binary_integer )

          // these keywords must come before identifier or they'd be matched by that
          ( R"(if)", token_id::if_ )
          ( R"(else)", token_id::else_ )
          ( R"(while)", token_id::while_ )
          ( R"(return)", token_id::return_ )
          ( R"(\w+)", token_id::identifier ) // alphanumeric and underscore

          ( R"(:)", token_id::colon )
          ( R"(;)", token_id::semicolon )
          ( R"(\.)", token_id::dot )
          ( R"(,)", token_id::comma )
          ( R"(=)", token_id::equals ) // must come before operator_ definition, since it's also covered by that
          ( R"(`)", token_id::backtick )
          ( R"(\()", token_id::paren_open )
          ( R"(\))", token_id::paren_close )
          ( R"(\{)", token_id::brace_open )
          ( R"(\})", token_id::brace_close )
          ( R"(\[)", token_id::square_bracket_open )
          ( R"(\])", token_id::square_bracket_close )
          // + - * . / | \ ! # $ % & < = > ? @ ^ ~
          ( R"([\+\-\*\.\/\|\\\!#$%&<=>\?@\^~]+)", token_id::operator_identifier )
          ;
      }
    };

    typedef token_definitions::iterator_type token_iterator;
  }
}
