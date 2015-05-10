#include "vm/stack.hpp"
#include "vm/exceptions.hpp"

#include <boost/test/unit_test.hpp>
#include <array>

// Testing the stack

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( stack )

BOOST_AUTO_TEST_CASE( stack_test )
{
  perseus::detail::stack stack;
  stack.reserve( sizeof( int ) + 1 );

  BOOST_CHECK_EQUAL( 0u, stack.size() );
  BOOST_CHECK( stack.empty() );

  stack.push< char >( 'x' );
  BOOST_CHECK_EQUAL( 1u, stack.size() );
  BOOST_CHECK( !stack.empty() );

  stack.push< int >( 42 );
  BOOST_CHECK_EQUAL( 1 + sizeof( int ), stack.size() );

  BOOST_CHECK_EQUAL( 42, stack.pop< int >() );
  BOOST_CHECK_EQUAL( 1, stack.size() );

  typedef std::array< char, 2 > two_bytes;
  BOOST_CHECK_THROW( stack.pop< two_bytes >(), perseus::stack_underflow );
  BOOST_CHECK_EQUAL( 1, stack.size() );

  BOOST_CHECK_EQUAL( 'x', stack.pop< char >() );
  BOOST_CHECK_EQUAL( 0, stack.size() );
  BOOST_CHECK( stack.empty() );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
