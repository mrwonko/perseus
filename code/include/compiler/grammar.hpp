#pragma once

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/lex.hpp>

#include <string>

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


    class grammar : public boost::spirit::qi::grammar< token_iterator, /* ast::file, */ skip_grammar >
    {
    public:
      grammar()
        : base_type( file, "perseus script"s )
      {
        file %= -byte_order_mark >> boost::spirit::qi::omit[ *( string | identifier ) ];
      }
    private:
      template< typename attribute > using rule = boost::spirit::qi::rule< token_iterator, attribute, skip_grammar >;

      //    terminals
      rule< std::string() > identifier = { boost::spirit::qi::token( token_id::identifier ), "identifier"s };
      rule< parsed_string_literal() > string = rule< parsed_string_literal() >( boost::spirit::qi::token( token_id::string ), "string"s ); // note that this must explicitly be std::string or it would be interpretted as an Expression (rule)
      rule< boost::spirit::unused_type() > byte_order_mark = rule< boost::spirit::unused_type() >( boost::spirit::qi::token( token_id::byte_order_mark ), "UTF-8 byte order mark"s );

      //    non-terminals; defined in constructor since they reference each other
      start_type file;//{ std::string( "file" ) };
    };
  }
}
