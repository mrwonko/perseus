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
using perseus::execution_finished;
using perseus::stack;
using perseus::stack_underflow;

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_SUITE( basic )

BOOST_AUTO_TEST_CASE( exit_0_test )
{
  BOOST_MESSAGE( "simple program that just exits, returning nothing" );
  auto code = create_code_segment( opcode::exit, std::uint32_t( 0 ) );
  processor proc( std::move( code ) );
  auto result = proc.continue_execution();
  BOOST_CHECK( result.execution_finished );
  BOOST_CHECK_EQUAL( result.stack.size(), 0 );
  BOOST_CHECK_THROW( proc.continue_execution(), execution_finished );
}

BOOST_AUTO_TEST_CASE( exit_2_test )
{
  BOOST_MESSAGE( "program that exits returning 2 bytes" );
  auto code = create_code_segment( opcode::exit, std::uint32_t( 2 ) );
  stack input;
  input.push< std::array< char, 4 > >( { 'a', 'b', 'c', 'd' } );
  auto result = processor( std::move( code ) ).continue_execution( std::move( input ) );
  return;
  BOOST_CHECK( result.execution_finished );
  BOOST_CHECK_EQUAL( result.stack.size(), 2 );
  BOOST_CHECK_EQUAL( result.stack.pop< char >(), 'd' );
  BOOST_CHECK_EQUAL( result.stack.pop< char >(), 'c' );
}

BOOST_AUTO_TEST_CASE( exit_underflow_test )
{
  BOOST_MESSAGE( "returning more on exit than is on the stack" );
  auto code = create_code_segment( opcode::exit, std::uint32_t( 2 ) );
  BOOST_CHECK_THROW( processor( std::move( code ) ).continue_execution(), stack_underflow );
}

BOOST_AUTO_TEST_CASE( exit_all_test )
{
  BOOST_MESSAGE( "returning everything on the stack on exit" );
  auto code = create_code_segment( opcode::exit_returning_everything );
  stack input;
  input.push< std::array< char, 4 > >( { 'a', 'b', 'c', 'd' } );
  auto result = processor( std::move( code ) ).continue_execution( std::move( input ) );
  BOOST_CHECK( result.execution_finished );
  BOOST_CHECK_EQUAL( result.stack.size(), 4 );
  BOOST_CHECK_EQUAL( result.stack.pop< char >(), 'd' );
  BOOST_CHECK_EQUAL( result.stack.pop< char >(), 'c' );
}

BOOST_AUTO_TEST_CASE( noop_test )
{
  BOOST_MESSAGE( "noop, then exit" );
  auto code = create_code_segment( opcode::no_operation, opcode::exit, std::uint32_t( 0 ) );
  BOOST_CHECK( processor( std::move( code ) ).continue_execution().execution_finished );
}

BOOST_AUTO_TEST_CASE( out_of_bounds_test )
{
  BOOST_MESSAGE( "code without exit, causing the instruction pointer to leave the code segment" );
  auto code = create_code_segment( opcode::no_operation );
  BOOST_CHECK_THROW( processor( std::move( code ) ).continue_execution(), code_segmentation_fault );
}

BOOST_AUTO_TEST_CASE( invalid_opcode_test )
{
  BOOST_MESSAGE( "code containing an invalid opcode" );
  auto code = create_code_segment( opcode::opcode_end );
  BOOST_CHECK_THROW( processor( std::move( code ) ).continue_execution(), invalid_opcode );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
