#include "compiler/conversions.hpp"
#include "compiler/exceptions.hpp"
#include "compiler/token_definitions.hpp"
#include "util/u32string_ostream.hpp"

#include <boost/spirit/home/lex/tokenize_and_parse.hpp>
#include <boost/spirit/home/qi.hpp>

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <cstdint>

using namespace std::string_literals;

static std::u32string parse_string( const std::string& string_literal )
{
  std::stringstream source( string_literal );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  std::u32string result;

  return boost::spirit::lex::tokenize_and_parse( begin, end, perseus::detail::token_definitions{}, perseus::detail::string_literal_parser{}, result )
    ? result
    : U"parse failed"s;
}

static std::int32_t parse_int( const std::string& int_literal )
{
  std::stringstream source( int_literal );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  typedef boost::spirit::qi::rule< perseus::detail::token_iterator, std::int32_t() > rule;
  rule dec{ perseus::detail::decimal_integer_literal_parser{}, "dec int"s };
  rule hex{ perseus::detail::hexadecimal_integer_literal_parser{}, "hex int"s };
  rule bin{ perseus::detail::binary_integer_literal_parser{}, "bin int"s };
  rule int_parser{ dec | hex | bin, "int"s };

  std::int32_t result;
  return boost::spirit::lex::tokenize_and_parse( begin, end, perseus::detail::token_definitions{}, int_parser, result )
    ? result
    : -1;
}

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( string_literals )

BOOST_AUTO_TEST_CASE( empty )
{
  BOOST_CHECK_EQUAL( parse_string( u8R"("")"s ), U""s );
}

BOOST_AUTO_TEST_CASE( null )
{
  BOOST_CHECK_EQUAL( parse_string( u8"\"\0\""s ), U"\0"s );
}

BOOST_AUTO_TEST_CASE( two_bytes )
{
  BOOST_CHECK_EQUAL( parse_string( u8"\"\u0080\""s ), U"\u0080"s );
}

BOOST_AUTO_TEST_CASE( three_bytes )
{
  BOOST_CHECK_EQUAL( parse_string( u8"\"\u0800\""s ), U"\u0800"s );
}

BOOST_AUTO_TEST_CASE( four_bytes )
{
  BOOST_CHECK_EQUAL( parse_string( u8"\"\U00010000\""s ), U"\U00010000"s );
}

BOOST_AUTO_TEST_CASE( stray_continuation )
{
  BOOST_CHECK_THROW( parse_string( "\"\x80\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( missing_continuation )
{
  BOOST_CHECK_THROW( parse_string( "\"\xC0_\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( too_large )
{
  BOOST_CHECK_THROW( parse_string( "\"\xf7\xbf\xbf\xbf\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_CASE( too_many_bytes )
{
  // 0xF9 = 0b1111'1001, 5 bytes
  BOOST_CHECK_THROW( parse_string( "\"\xf9\x80\x80\x80\x80\""s ), perseus::invalid_utf8 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( integer_literals )

BOOST_AUTO_TEST_CASE( null )
{
  BOOST_CHECK_EQUAL( parse_int( "0"s ), 0 );
}

BOOST_AUTO_TEST_CASE( leet )
{
  BOOST_CHECK_EQUAL( parse_int( "1337"s ), 1337 );
}

BOOST_AUTO_TEST_CASE( separator )
{
  BOOST_CHECK_EQUAL( parse_int( "1'2'3'4"s ), 1234 );
}

BOOST_AUTO_TEST_CASE( binary )
{
  BOOST_CHECK_EQUAL( parse_int( "0b1001'1001"s ), 0b1001'1001 );
}

BOOST_AUTO_TEST_CASE( hex )
{
  BOOST_CHECK_EQUAL( parse_int( "0xBAD'F00D"s ), 0xBAD'F00D );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
