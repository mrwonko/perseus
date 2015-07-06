#include "compiler/iterators.hpp"
#include "compiler/token_definitions.hpp"
#include "compiler/grammar.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( parser )

BOOST_AUTO_TEST_CASE( parse )
{
  std::stringstream source( u8R"(
identifier _hi3 "foo"
)" );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  bool success = boost::spirit::lex::tokenize_and_phrase_parse( begin, end, perseus::detail::token_definitions{}, perseus::detail::grammar{}, perseus::detail::skip_grammar{} );

  BOOST_CHECK( success );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
