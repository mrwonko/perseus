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

    The first phase of perseus compilation is tokenization, or lexical analysis. The char stream
    */
    template< typename lexer >
    class tokens : public boost::spirit::lex::lexer< lexer >
    {
    public:
      /// 
      tokens()
      {
        self = identifier | whitespace | comment;
      }

      template< typename Attribute = boost::spirit::unused_type > using token_def = boost::spirit::lex::token_def< Attribute, char, token_id::token_id >;

      token_def< std::string > identifier{ R"([a-zA-Z_][a-zA-Z_0-9]*)", token_id::identifier };
      token_def<> whitespace{ R"([ \t\n]+)", token_id::whitespace };
      token_def<> comment{ R"(\/\/[^\n]*)", token_id::comment };
    };
  }
}
