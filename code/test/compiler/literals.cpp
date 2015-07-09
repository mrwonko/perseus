#include "compiler/conversions.hpp"
#include "compiler/exceptions.hpp"
#include "compiler/token_definitions.hpp"
#include "util/u32string_ostream.hpp"

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>

#include <boost/test/unit_test.hpp>

#include <sstream>

using namespace std::string_literals;

static std::u32string parse( const std::string& string_literal )
{
  std::stringstream source( string_literal );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  std::u32string result;

  return boost::spirit::lex::tokenize_and_parse( begin, end, perseus::detail::token_definitions{}, perseus::detail::string_literal_parser{}, result )
    ? result
    : U"parse failed"s;
}

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( literals )

BOOST_AUTO_TEST_CASE( empty )
{
  BOOST_CHECK_EQUAL( parse( u8R"("")"s ), U""s );
}

BOOST_AUTO_TEST_CASE( null )
{
  BOOST_CHECK_EQUAL( parse( u8"\"\0\""s ), U"\0"s );
}

BOOST_AUTO_TEST_CASE( two_bytes )
{
  BOOST_CHECK_EQUAL( parse( u8"\"\u0080\""s ), U"\u0080"s );
}

BOOST_AUTO_TEST_CASE( three_bytes )
{
  BOOST_CHECK_EQUAL( parse( u8"\"\u0800\""s ), U"\u0800"s );
}

BOOST_AUTO_TEST_CASE( four_bytes )
{
  BOOST_CHECK_EQUAL( parse( u8"\"\U00010000\""s ), U"\U00010000"s );
}

BOOST_AUTO_TEST_CASE( stray_continuation )
{
  BOOST_CHECK_THROW( parse( "\"\x80\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( missing_continuation )
{
  BOOST_CHECK_THROW( parse( "\"\xC0_\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( too_large )
{
  BOOST_CHECK_THROW( parse( "\"\xf7\xbf\xbf\xbf\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( too_many_bytes )
{
  // 0xF9 = 0b1111'1001, 5 bytes
  BOOST_CHECK_THROW( parse( "\"\xf9\x80\x80\x80\x80\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
