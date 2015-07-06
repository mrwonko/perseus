#include "compiler/iterators.hpp"
#include "compiler/tokens.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/lex/lexer/lexertl/token.hpp>
#include <boost/spirit/home/lex/lexer/lexertl/lexer.hpp>
#include <boost/spirit/home/lex/lexer_lexertl.hpp>

#include <sstream>
#include <vector>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( lexer )

BOOST_AUTO_TEST_CASE( tokenize )
{
  std::stringstream source( u8R"(// a cömment
	some identifiers)" );

  typedef perseus::detail::enhanced_istream_iterator iterator;
  typedef boost::spirit::lex::lexertl::token<
    iterator,
    boost::mpl::vector< std::string >, // all the attribute types used (returned) by the various token definitions
    boost::mpl::false_, // not interested in lexer states
    perseus::detail::token_id::token_id // token type
  > token;
  typedef boost::spirit::lex::lexertl::lexer< token > lexer;
  typedef perseus::detail::tokens< lexer > tokens;

  tokens t;

  iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  std::vector< token > result;

  bool success = boost::spirit::lex::tokenize(
    begin,
    end,
    t,
    [ &result ]( const token& t )
  {
    result.push_back( t );
    return true; // continue tokenizing
  }
  );

  BOOST_CHECK( success );

  BOOST_CHECK_EQUAL( result.size(), 5 );
  BOOST_CHECK_EQUAL( result.at( 0 ).id(), perseus::detail::token_id::comment );
  BOOST_CHECK_EQUAL( result.at( 1 ).id(), perseus::detail::token_id::whitespace );
  BOOST_CHECK_EQUAL( result.at( 2 ).id(), perseus::detail::token_id::identifier );
  // this works, but I don't think I should test for it - spirit::lex initially assigns iterator_range to the value
  //BOOST_CHECK_EQUAL( boost::get< boost::iterator_range< iterator > >( result.at( 2 ).value() ).begin().get_position(), perseus::detail::file_position( 2, 2 ) );
  // this does not work, the on-demand conversion of the iterator_range to the target value does not seem to happen until spirit::qi parses it
  //BOOST_CHECK_EQUAL( boost::get< std::string >( result.at( 2 ).value() ), "some" );
  BOOST_CHECK_EQUAL( result.at( 3 ).id(), perseus::detail::token_id::whitespace );
  BOOST_CHECK_EQUAL( result.at( 4 ).id(), perseus::detail::token_id::identifier );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
