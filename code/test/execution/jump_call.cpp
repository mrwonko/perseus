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

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_SUITE( jump )

BOOST_AUTO_TEST_CASE( absolute_jump )
{
  auto code = create_code_segment( 
    opcode::absolute_jump, label_reference( "label" ),
    opcode::push_32, std::uint32_t( 1337 ), // jumped over, not executed
    label( "label" ),
    opcode::exit );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( relative_jump_forward )
{
  auto code = create_code_segment(
    opcode::relative_jump, label_reference_offset( "label" ),
    opcode::push_32, std::uint32_t( 1337 ), // jumped over, not executed
    label( "label" ),
    opcode::exit );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( relative_jump_backwards )
{
  auto code = create_code_segment(
    opcode::absolute_jump, label_reference( "label1" ),

    opcode::push_32, std::uint32_t( 1337 ), // jumped over, not executed

    label( "label2" ),
    opcode::push_32, std::uint32_t( 42 ),
    opcode::exit,

    label( "label1" ),
    opcode::relative_jump, label_reference_offset( "label2" )
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 42 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( indirect_jump )
{
  auto code = create_code_segment(
    opcode::push_32, label_reference( "label" ),
    opcode::indirect_jump,
    opcode::push_32, std::uint32_t( 1337 ), // jumped over, not executed
    label( "label" ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( relative_jump_if_false )
{
  auto pushCode = create_code_segment( opcode::push_32, std::uint32_t( 1337 ) );
  auto code = create_code_segment(
    opcode::push_8, std::uint8_t( false ),
    opcode::relative_jump_if_false, label_reference_offset( "label1" ),
    opcode::push_32, std::uint32_t( 42 ), // jumped over, not executed
    label( "label1" ),

    opcode::push_8, std::uint8_t( true ),
    opcode::relative_jump_if_false, label_reference_offset( "label2" ),
    opcode::push_32, std::uint32_t( 1337 ),
    label( "label2" ),

    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 1337 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( indirect_call )
{
  std::uint32_t return_address = 0;
  auto code = create_code_segment(
    opcode::push_32, label_reference( "push_and_exit" ),
    opcode::indirect_call,
    get_current_address( return_address ),
    opcode::push_32, std::uint32_t( 42 ),
    opcode::exit,

    label( "push_and_exit" ),
    opcode::push_32, std::uint32_t( 1337 ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 1337 );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), return_address );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( call_and_return_factorial )
{
  auto code = create_code_segment(
    // stack: [-4: n] (supplied by executor)
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -4 ),
    // stack: [-8: n (return value memory, to be overwritten)] [-4: n]
    // result = factorial( n )
    opcode::call, label_reference( "factorial" ),
    // stack: [factorial( n )]
    opcode::exit,

    label( "factorial" ),
    // stack: [-12: return_value] [-8: n] [-4: return_address]

    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -8 ),
    // stack: [-16: return_value] [-12: n] [-8: return_address] [-4: n]

    opcode::push_32, std::int32_t( 1 ),
    // stack: [-20: return_value] [-16: n] [-12: return_address] [-8: n] [-4: 1]

    opcode::less_than_or_equals_i32,
    // stack: [-13: return_value] [-9: n] [-5: return_address] [-1: n <= 1]

    opcode::relative_jump_if_false, label_reference_offset( "n > 1" ),
    // stack: [-12: return_value] [-8: n] [-4: return_address]

    //    n <= 1: return 1
    opcode::push_32, std::int32_t( 1 ),
    // stack: [-16: return_value] [-12: n] [-8: return_address] [-4: 1]
    opcode::relative_jump, label_reference_offset( "return_top_of_stack" ),

    //    n > 1: return n * factorial( n - 1)
    label( "n > 1" ),
    // stack: [-12: return_value] [-8: n] [-4: return_address]
    opcode::reserve, std::uint32_t( 4 ),
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -12 ),
    // stack: [-20: return_value] [-16: n] [-12: return_address] [-8: 0] [-4: n]
    opcode::push_32, std::int32_t( 1 ),
    opcode::subtract_i32,
    // stack: [-20: return_value] [-16: n] [-12: return_address] [-8: 0] [-4: n-1]
    opcode::call, label_reference( "factorial" ),
    // stack: [-16: return_value] [-12: n] [-8: return_address] [-4: factorial(n-1)]
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -12 ),
    // stack: [-20: return_value] [-16: n] [-12: return_address] [-8: factorial(n-1)] [-4: n]
    opcode::multiply_i32,
    // stack: [-16: return_value] [-12: n] [-8: return_address] [-4: factorial(n-1)*n]

    label( "return_top_of_stack" ),
    // stack: [-16: return_value] [-12: n] [-8: return_address] [-4: result]
    opcode::relative_store_stack, std::uint32_t( 4 ), std::int32_t( -16 ), // store result in return_value
    opcode::pop, std::uint32_t( 4 ), // pop result from top of stack
    // stack: [-12: return_value = result] [-8: n] [-4: return_address]
    opcode::return_, std::uint32_t( 4 ) // return, popping 4 bytes of parameters
    );
  perseus::stack result = processor( perseus::detail::code_segment( code ) ).execute( 0, std::move( perseus::stack().push< std::int32_t >( 5 ) ) );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 120 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
  result = processor( std::move( code ) ).execute( 0, std::move( perseus::stack().push< std::int32_t >( 0 ) ) );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 1 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
