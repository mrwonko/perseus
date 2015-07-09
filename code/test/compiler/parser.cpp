#include "compiler/grammar.hpp"

#include "util/u32string_ostream.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/variant/get.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( parser )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( parse )
{
  namespace ast = perseus::detail::ast;
  std::stringstream source( u8R"(

my_identifier42

)" );

  perseus::detail::enhanced_istream_iterator source_begin, source_end;
  std::tie( source_begin, source_end ) = perseus::detail::enhanced_iterators( source );

  perseus::detail::token_definitions tokens;

  perseus::detail::token_iterator tokens_it = tokens.begin( source_begin, source_end );
  perseus::detail::token_iterator tokens_end = tokens.end();

  ast::expression result;
  bool success = boost::spirit::qi::phrase_parse( tokens_it, tokens_end, perseus::detail::grammar{}, perseus::detail::skip_grammar{}, result );

  BOOST_CHECK( success );
  BOOST_CHECK( tokens_it == tokens_end );

  BOOST_CHECK_EQUAL( result.tail.size(), 0 );
  ast::operand* operand = &result.head;
  ast::identifier* identifier = boost::get< ast::identifier >( operand );
  BOOST_REQUIRE( identifier );
  BOOST_CHECK_EQUAL( *identifier, "my_identifier42" );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
