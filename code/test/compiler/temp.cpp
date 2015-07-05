#include "compiler/compile.hpp"

#include <boost/test/unit_test.hpp>

#include <sstream>

// Testing the stack

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( temp )

BOOST_AUTO_TEST_CASE( compile )
{
  BOOST_MESSAGE( "invoke compilation; it currently outputs debug data, which I am interested in." );
  std::stringstream ss;
  ss << u8"Hello\noh W\u00F6rld!";
  perseus::compile( ss, "<string>" );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
