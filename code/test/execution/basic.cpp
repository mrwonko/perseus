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
using perseus::invalid_opcode;
using perseus::code_segmentation_fault;
using perseus::stack;
using perseus::stack_underflow;

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_SUITE( basic )

BOOST_AUTO_TEST_CASE( exit_0_test )
{
  BOOST_MESSAGE( "simple program that just exits, returning nothing" );
  auto code = create_code_segment( opcode::exit );
  auto result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( exit_4_test )
{
  BOOST_MESSAGE( "program that exits returning 4 bytes" );
  auto code = create_code_segment( opcode::exit );
  stack input;
  input.push< std::array< char, 4 > >( { 'a', 'b', 'c', 'd' } );
  processor proc( std::move( code ) );
  auto result = proc.execute( 0u, std::move( input ) );
  BOOST_CHECK( !proc.has_coroutines() );
  BOOST_CHECK_EQUAL( result.size(), 4 );
  BOOST_CHECK_EQUAL( result.pop< char >(), 'd' );
  BOOST_CHECK_EQUAL( result.pop< char >(), 'c' );
}
BOOST_AUTO_TEST_CASE( noop_test )
{
  BOOST_MESSAGE( "noop, then exit" );
  auto code = create_code_segment( opcode::no_operation, opcode::exit, std::uint32_t( 0 ) );
  processor proc( std::move( code ) );
  BOOST_CHECK_EQUAL( proc.execute().size(), 0u );
  BOOST_CHECK( !proc.has_coroutines() );
}

BOOST_AUTO_TEST_CASE( out_of_bounds_test )
{
  BOOST_MESSAGE( "code without exit, causing the instruction pointer to leave the code segment" );
  auto code = create_code_segment( opcode::no_operation );
  BOOST_CHECK_THROW( processor( std::move( code ) ).execute(), code_segmentation_fault );
}

BOOST_AUTO_TEST_CASE( invalid_opcode_test )
{
  BOOST_MESSAGE( "code containing an invalid opcode" );
  auto code = create_code_segment( opcode::opcode_end );
  BOOST_CHECK_THROW( processor( std::move( code ) ).execute(), invalid_opcode );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
