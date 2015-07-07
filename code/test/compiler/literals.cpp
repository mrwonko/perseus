#include "compiler/conversions.hpp"
#include "../u32string_ostream.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>

static std::u32string parse( const char* string_literal )
{
  std::stringstream source( string_literal );

  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  return perseus::detail::parsed_string_literal( begin, end );
}

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( literals )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( empty )
{
  std::u32string parsed = parse( u8R"("")" );

  BOOST_CHECK_EQUAL( parsed, U""s );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
