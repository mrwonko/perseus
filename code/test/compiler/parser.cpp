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

"hello world"

)" );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  std::u32string result;
  bool success = boost::spirit::lex::tokenize_and_phrase_parse( begin, end, perseus::detail::token_definitions{}, perseus::detail::grammar{}, perseus::detail::skip_grammar{}, result );

  BOOST_CHECK( success );
  BOOST_CHECK_EQUAL( result, U"hello world"s );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
