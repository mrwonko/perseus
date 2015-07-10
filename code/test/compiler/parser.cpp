#include "compiler/compiler.hpp"

#include "util/u32string_ostream.hpp"

#include <boost/test/unit_test.hpp>

#include <boost/spirit/home/qi/parse.hpp>
#include <boost/variant/get.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( parser )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( parse )
{
  namespace ast = perseus::detail::ast;

  ast::expression result = perseus::compiler().parse( std::stringstream( "  my_identifier42  " ), "<string>" );

  BOOST_CHECK_EQUAL( result.tail.size(), 0 );
  ast::operand* operand = &result.head;
  ast::identifier* identifier = boost::get< ast::identifier >( operand );
  BOOST_REQUIRE( identifier );
  BOOST_CHECK_EQUAL( *identifier, "my_identifier42" );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
