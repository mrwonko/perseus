#include "compiler/iterators.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( iterators )

BOOST_AUTO_TEST_CASE( position_iterator )
{
  using perseus::detail::file_position;
  using perseus::detail::enhanced_istream_iterator;
  using perseus::detail::enhanced_iterators;

  std::stringstream source{ u8"\u00E4b\nc" };
  enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = enhanced_iterators( source );

  auto it = begin;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 1 ) );
  BOOST_CHECK_EQUAL( *it, u8"\u00E4"[ 0 ] );

  ++it;
  BOOST_REQUIRE( it != end );
  // this is the second byte of a multi-byte encoded single character - the counter should be aware of this and still report column 1
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 1 ) );
  BOOST_CHECK_EQUAL( *it, u8"\u00E4"[ 1 ] );

  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 2 ) );
  BOOST_CHECK_EQUAL( *it, 'b' );

  auto it2 = it;
  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 1, 3 ) );
  BOOST_CHECK_EQUAL( *it, '\n' );
  BOOST_REQUIRE( it2 != end );
  BOOST_CHECK_EQUAL( it2.get_position(), file_position( 1, 2 ) );
  BOOST_CHECK_EQUAL( *it2, 'b' );

  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( it.get_position(), file_position( 2, 1 ) );
  BOOST_CHECK_EQUAL( *it, 'c' );

  ++it;
  BOOST_CHECK( it == end );
}

BOOST_AUTO_TEST_CASE( position_iterator_bom )
{
  using perseus::detail::file_position;
  using perseus::detail::enhanced_istream_iterator;
  using perseus::detail::enhanced_iterators;

  std::stringstream source{ u8"\uFEFF" };
  enhanced_istream_iterator begin, end;
  std::tie( begin, end ) = enhanced_iterators( source );

  auto it = begin;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( *it, '\xef' );
  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( *it, '\xbb' );
  ++it;
  BOOST_REQUIRE( it != end );
  BOOST_CHECK_EQUAL( *it, '\xbf' );
  ++it;
  BOOST_CHECK( it == end );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
