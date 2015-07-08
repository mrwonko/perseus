#include "compiler/iterators.hpp"
#include "compiler/token_definitions.hpp"
#include "compiler/grammar.hpp"

#include "../u32string_ostream.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( parser )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( parse )
{
  std::stringstream source( u8R"(

1337 0x42 0b1100'0001

)" );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  std::vector< std::int32_t > result;
  bool success = boost::spirit::lex::tokenize_and_phrase_parse( begin, end, perseus::detail::token_definitions{}, perseus::detail::grammar{}, perseus::detail::skip_grammar{}, result );

  BOOST_CHECK( success );
  BOOST_CHECK_EQUAL( result.size(), 3 );
  BOOST_CHECK_EQUAL( result.at( 0 ), 1337 );
  BOOST_CHECK_EQUAL( result.at( 1 ), 0x42 );
  BOOST_CHECK_EQUAL( result.at( 2 ), 0b1100'0001 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
