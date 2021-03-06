#include "compiler/iterators.hpp"
#include "compiler/token_definitions.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>

#include <sstream>
#include <vector>

static bool tokenize( const std::string& source, std::vector< perseus::detail::token >& out_tokens )
{
  std::stringstream stream( source );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( stream );

  return boost::spirit::lex::tokenize(
    begin,
    end,
    perseus::detail::token_definitions{},
    [ &out_tokens ]( const perseus::detail::token& t )
  {
    out_tokens.push_back( t );
    return true; // continue tokenizing
  }
  );
}

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( lexer )

BOOST_AUTO_TEST_CASE( basic )
{
  std::vector< perseus::detail::token > result;
  bool success = tokenize( u8R"(// a c\u00D6mment
	some identifiers/*
and a long comment
*/)", result );

  BOOST_CHECK( success );

  BOOST_CHECK_EQUAL( result.size(), 6 );
  BOOST_CHECK_EQUAL( result.at( 0 ).id(), perseus::detail::token_id::comment );
  BOOST_CHECK_EQUAL( result.at( 1 ).id(), perseus::detail::token_id::whitespace );
  BOOST_CHECK_EQUAL( result.at( 2 ).id(), perseus::detail::token_id::identifier );
  BOOST_CHECK_EQUAL( result.at( 2 ).value().begin().get_position(), perseus::detail::file_position( 2, 2 ) );
  BOOST_CHECK_EQUAL( std::string( result.at( 2 ).value().begin(), result.at( 2 ).value().end() ), "some" );
  BOOST_CHECK_EQUAL( result.at( 3 ).id(), perseus::detail::token_id::whitespace );
  BOOST_CHECK_EQUAL( result.at( 4 ).id(), perseus::detail::token_id::identifier );
  BOOST_CHECK_EQUAL( result.at( 5 ).id(), perseus::detail::token_id::comment );
}

BOOST_AUTO_TEST_CASE( longest_match_if )
{
  std::vector< perseus::detail::token > result;
  bool success = tokenize( u8R"(iffy if)", result );

  BOOST_CHECK( success );

  BOOST_CHECK_EQUAL( result.size(), 3 );
  BOOST_CHECK_EQUAL( result.at( 0 ).id(), perseus::detail::token_id::identifier );
  BOOST_CHECK_EQUAL( result.at( 1 ).id(), perseus::detail::token_id::whitespace );
  BOOST_CHECK_EQUAL( result.at( 2 ).id(), perseus::detail::token_id::if_ );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
