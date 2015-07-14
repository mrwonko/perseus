#include "compiler/compiler.hpp"
#include "compiler/ast.hpp"

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

  // TODO: update to new syntax
  /*
  ast::parser::file result = perseus::compiler().parse( std::stringstream( "function f() 42" ), "<string>" );

  BOOST_CHECK_EQUAL( result.functions.size(), 1 );
  const ast::parser::function_definition& func = result.functions.front();
  BOOST_CHECK_EQUAL( func.name, "f" );
  BOOST_CHECK( !func.type );
  BOOST_CHECK_EQUAL( func.arguments.size(), 0 );
  BOOST_CHECK_EQUAL( func.body.tail.size(), 0 );
  const std::int32_t* body = boost::get< std::int32_t >( &func.body.head );
  BOOST_REQUIRE( body );
  BOOST_CHECK_EQUAL( *body, 42 );
  */
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
