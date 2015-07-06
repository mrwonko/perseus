#pragma once

#include <boost/spirit/home/qi.hpp>
#include <boost/spirit/home/lex.hpp>

#include "iterators.hpp"
#include "ast.hpp"
#include "token_ids.hpp"
#include "token_definitions.hpp"

namespace perseus
{
  namespace detail
  {
    using iterator = token_definitions::iterator_type;

    class skip_grammar : public boost::spirit::qi::grammar< iterator >
    {
    public:
      skip_grammar()
        : base_type( skip )
      {
        skip = whitespace | comment;
      }
    private:
      using rule = boost::spirit::qi::rule< iterator >;

      // terminals
      rule whitespace = { boost::spirit::qi::token( token_id::whitespace ), "whitespace" };
      rule comment = { boost::spirit::qi::token( token_id::whitespace ), "comment" };
      // start symbol
      start_type skip;
    };

    class grammar : public boost::spirit::qi::grammar< iterator, /* ast::file, */ skip_grammar >
    {
    public:
      grammar()
        : base_type( file, "perseus script" )
      {
        file %= *( string | identifier );
      }
    private:
      template< typename attribute > using rule = boost::spirit::qi::rule< iterator, attribute, skip_grammar >;

      //    terminals
#define PERSEUS_DETAIL_DEFINE_TERMINAL( name ) rule< boost::spirit::unused_type > name = { boost::spirit::qi::token( token_id::name ), #name }
      PERSEUS_DETAIL_DEFINE_TERMINAL( string );
      PERSEUS_DETAIL_DEFINE_TERMINAL( identifier );
#undef PERSEUS_DETAIL_DEFINE_TERMINAL

      //    non-terminals; defined in constructor since they reference each other
      start_type file;//{ std::string( "file" ) };
    };
  }
}
