#include "vm/stack.hpp"
#include "vm/exceptions.hpp"
#include "vm/processor.hpp"

#include <boost/test/unit_test.hpp>
#include <array>
#include <utility>

// Testing the stack

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( stack )

BOOST_AUTO_TEST_CASE( push_pop_test )
{
  perseus::stack stack;
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

BOOST_AUTO_TEST_CASE( move_test )
{
  perseus::stack stack1;
  stack1.push< int >( 32 );
  // move constructor
  perseus::stack stack2( std::move( stack1 ) );
  BOOST_CHECK( stack1.empty() );
  BOOST_CHECK( stack2.size() == sizeof( int ) );
  perseus::stack stack3;
  // move assignment
  stack3 = std::move( stack2 );
  BOOST_CHECK( stack2.empty() );
  BOOST_CHECK( stack3.size() == sizeof( int ) );
}

BOOST_AUTO_TEST_CASE( append_test )
{
  perseus::stack stack1, stack2;

  stack1.push< char >( 'a' );
  stack2.push< char >( 'b' );
  stack1.append( stack2 );
  BOOST_CHECK_EQUAL( stack1.size(), 2 );
  BOOST_CHECK_EQUAL( stack2.size(), 1 );
  BOOST_CHECK_EQUAL( stack1.pop< char >(), 'b' );
  BOOST_CHECK_EQUAL( stack1.pop< char >(), 'a' );
  BOOST_CHECK( stack1.empty() );
  BOOST_CHECK_EQUAL( stack2.pop< char >(), 'b' );
  BOOST_CHECK( stack2.empty() );
}

BOOST_AUTO_TEST_CASE( split_test )
{
  perseus::stack stack1;
  stack1.push< std::array< char, 4 > >( { 'a', 'b', 'c', 'd' } );
  perseus::stack stack2 = stack1.split( 3 );

  BOOST_CHECK_EQUAL( stack1.size(), 1 );
  BOOST_CHECK_EQUAL( stack2.size(), 3 );
  BOOST_CHECK_EQUAL( stack1.pop< char >(), 'a' );
  BOOST_CHECK_EQUAL( stack2.pop< char >(), 'd' );
  BOOST_CHECK_EQUAL( stack2.size(), 2 );
  BOOST_CHECK_THROW( stack2.split( 3 ), perseus::stack_underflow );
  BOOST_CHECK_EQUAL( stack2.size(), 2 );
  BOOST_CHECK_EQUAL( stack2.pop< char >(), 'c' );
}

BOOST_AUTO_TEST_CASE( split_0_test )
{
  perseus::stack stack1;
  perseus::stack stack2 = stack1.split( 0 );

  BOOST_CHECK_EQUAL( stack1.size(), 0 );
  BOOST_CHECK_EQUAL( stack2.size(), 0 );
}

BOOST_AUTO_TEST_CASE( split_0_test_2 )
{
  perseus::stack stack1;
  stack1.push< char >( 'a' );
  perseus::stack stack2 = stack1.split( 0 );

  BOOST_CHECK_EQUAL( stack1.size(), 1 );
  BOOST_CHECK_EQUAL( stack2.size(), 0 );
}

// This happens in the context of the processor processing opcode::exit* and leaked memory at some point. Isolating it in a small testcase to fix it.
static perseus::detail::processor::execution_result mk_leaky_result()
{
  perseus::stack stack1;
  return{ true, std::move( stack1 ) };
}

BOOST_AUTO_TEST_CASE( leaky_result )
{
  perseus::detail::processor::execution_result result = mk_leaky_result();
  BOOST_CHECK( result.stack.size() == 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
