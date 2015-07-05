#include "compiler/iterators.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <iostream>
#include <iomanip>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( iterators )

BOOST_AUTO_TEST_CASE( compile )
{
  using perseus::detail::file_position;
  using perseus::detail::enhanced_istream_iterator;

  std::stringstream source( "ab\nc" );
  perseus::detail::enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = perseus::detail::enhanced_iterators( source );

  auto it = begin;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 1 ) );
  BOOST_CHECK_EQUAL( *it, U'a' );

  auto it2 = it;
  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 2 ) );
  BOOST_CHECK_EQUAL( *it, U'b' );
  BOOST_REQUIRE( it2 != end );
  BOOST_CHECK_EQUAL( it2.get_position(), file_position( 1, 1 ) );
  BOOST_CHECK_EQUAL( *it2, U'a' );

  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 3 ) );
  BOOST_CHECK_EQUAL( *it, U'\n' );

  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 2, 1 ) );
  BOOST_CHECK_EQUAL( *it, U'c' );

  ++it;
  BOOST_CHECK( it == end );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
