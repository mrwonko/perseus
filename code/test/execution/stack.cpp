#include "../write_code_segment.hpp"

#include "vm/processor.hpp"
#include "vm/stack.hpp"
#include "vm/exceptions.hpp"

#include <utility>
#include <cstdint>
#include <array>

#include <boost/test/unit_test.hpp>

using perseus::detail::processor;
using perseus::detail::opcode;
using perseus::stack_underflow;

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_SUITE( stack )

BOOST_AUTO_TEST_CASE( push_8_test )
{
  BOOST_MESSAGE( "pushing 8 bit on the stack" );
  auto code = create_code_segment( opcode::push_8, std::uint8_t( 42 ), opcode::exit );
  auto result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 1 );
  BOOST_CHECK_EQUAL( result.pop< std::uint8_t >(), 42 );
}

BOOST_AUTO_TEST_CASE( push_32_test )
{
  BOOST_MESSAGE( "pushing 32 bit on the stack" );
  auto code = create_code_segment( opcode::push_32, std::uint32_t( 1337 ), opcode::exit );
  auto result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 4 );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 1337 );
}


BOOST_AUTO_TEST_CASE( pop_test )
{
  BOOST_MESSAGE( "popping things off the stack" );
  auto code = create_code_segment( opcode::pop, std::uint32_t( 2 ), opcode::exit );
  perseus::stack initial_stack;
  initial_stack.push( std::array< char, 4 >{ 'a', 'b', 'c', 'd' } );
  auto result = processor( std::move( code ) ).execute( 0u, std::move( initial_stack ) );
  BOOST_CHECK_EQUAL( result.size(), 2 );
  BOOST_CHECK_EQUAL( result.pop< char >(), 'b' );
  BOOST_CHECK_EQUAL( result.pop< char >(), 'a' );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
