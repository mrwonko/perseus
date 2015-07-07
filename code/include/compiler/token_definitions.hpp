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
        self.add
          ( R"(\xEF\xBB\xBF)", token_id::byte_order_mark )
          ( R"(\s+)", token_id::whitespace )
          ( R"(\/\/[^\n]*|\/\*([^\*]|\*[^\/])*\*\/)", token_id::comment )
          ( R"(\"([^\n\"\\]|\\[^\n])*\")", token_id::string )
          ( R"(\w+)", token_id::identifier ) // alphanumeric and underscore
          ;
      }
    };
  }
}
