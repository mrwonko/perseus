#pragma once

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/lex.hpp>

#include <string>
#include <vector>

#include "iterators.hpp"
#include "ast.hpp"
#include "token_ids.hpp"
#include "token_definitions.hpp"
#include "conversions.hpp"

// DELETEME
#include <iostream>

namespace perseus
{
  namespace detail
  {
    using namespace std::string_literals; // gain access to operator ""s

    class skip_grammar : public boost::spirit::qi::grammar< token_iterator >
    {
    public:
      skip_grammar()
        : base_type( skip )
      {
        skip = whitespace | comment;
      }
    private:
      using rule = boost::spirit::qi::rule< token_iterator >;

      // terminals
      rule whitespace = { boost::spirit::qi::token( token_id::whitespace ), "whitespace"s };
      rule comment = { boost::spirit::qi::token( token_id::whitespace ), "comment"s };
      // start symbol
      start_type skip;
    };


    class grammar : public boost::spirit::qi::grammar< token_iterator, std::vector< std::int32_t >(), skip_grammar >
    {
    public:
      grammar()
        : base_type( file, "perseus script"s )
      {
        file %= ( -byte_order_mark ) >> *( decimal_integer | hexadecimal_integer | binary_integer );
        boost::spirit::qi::int_;
      }
    private:
      template< typename attribute = boost::spirit::unused_type() > using rule = boost::spirit::qi::rule< token_iterator, attribute, skip_grammar >;

      //    terminals

      rule<> byte_order_mark = { boost::spirit::qi::token( token_id::byte_order_mark ), "UTF-8 byte order mark"s };
      rule< parsed_string_literal() > string = { boost::spirit::qi::token( token_id::string ), "string"s };
      rule< std::int32_t() > decimal_integer = { decimal_integer_literal_parser{}, "decimal integer"s };
      rule< std::int32_t() > hexadecimal_integer = { hexadecimal_integer_literal_parser{}, "hexadecimal integer"s };
      rule< std::int32_t() > binary_integer = { binary_integer_literal_parser{}, "binary integer"s };

      //rule< ast::identifier() > identifier = { boost::spirit::qi::raw[ boost::spirit::qi::token( token_id::identifier ) ], "identifier"s };
      //rule< ast::operator_identifier() > operator_identifier = { boost::spirit::qi::token( token_id::operator_identifier ), "operator identifier"s };

      rule<> if_ = { boost::spirit::qi::token( token_id::if_ ), "if"s };
      rule<> else_ = { boost::spirit::qi::token( token_id::else_ ), "else"s };
      rule<> while_ = { boost::spirit::qi::token( token_id::while_ ), "while"s };
      rule<> return_ = { boost::spirit::qi::token( token_id::if_ ), "return"s };

      rule<> colon = { boost::spirit::qi::token( token_id::colon ), "colon"s };
      rule<> semicolon = { boost::spirit::qi::token( token_id::semicolon ), "semicolon"s };
      rule<> dot = { boost::spirit::qi::token( token_id::dot ), "dot"s };
      rule<> comma = { boost::spirit::qi::token( token_id::comma ), "comma"s };
      rule<> equals = { boost::spirit::qi::token( token_id::equals ), "equals sign"s };
      rule<> backtick = { boost::spirit::qi::token( token_id::backtick ), "backtick"s };

      rule<> paren_open = { boost::spirit::qi::token( token_id::paren_open ), "opening paren"s };
      rule<> paren_close = { boost::spirit::qi::token( token_id::paren_close ), "closing paren"s };
      rule<> brace_open = { boost::spirit::qi::token( token_id::brace_open ), "opening brace"s };
      rule<> brace_close = { boost::spirit::qi::token( token_id::brace_close ), "closing brace"s };
      rule<> square_bracket_open = { boost::spirit::qi::token( token_id::square_bracket_open ), "opening square bracket"s };
      rule<> square_bracket_close = { boost::spirit::qi::token( token_id::square_bracket_close ), "closing square bracket"s };

      //    non-terminals; defined in constructor since they reference each other
      start_type file;// { "file"s };
      //rule< ast::expression() > expression;
    };
  }
}
