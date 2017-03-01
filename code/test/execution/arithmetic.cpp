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

BOOST_AUTO_TEST_SUITE( boolean_arithmetic )

BOOST_AUTO_TEST_CASE( boolean_and )
{
  for( std::uint8_t a = 0; a < 2; ++a )
  {
    for( std::uint8_t b = 0; b < 2; ++b )
    {
      auto code = create_code_segment( opcode::and_b, opcode::exit );
      perseus::stack stack;
      stack.push< std::uint8_t >( a );
      stack.push< std::uint8_t >( b );
      perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
      BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), a && b );
      BOOST_CHECK_EQUAL( result.size(), 0 );
    }
  }
}

BOOST_AUTO_TEST_CASE( boolean_or )
{
  for( std::uint8_t a = 0; a < 2; ++a )
  {
    for( std::uint8_t b = 0; b < 2; ++b )
    {
      auto code = create_code_segment( opcode::or_b, opcode::exit );
      perseus::stack stack;
      stack.push< std::uint8_t >( a );
      stack.push< std::uint8_t >( b );
      perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
      BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), a || b );
      BOOST_CHECK_EQUAL( result.size(), 0 );
    }
  }
}

BOOST_AUTO_TEST_CASE( boolean_equals )
{
  for( std::uint8_t a = 0; a < 2; ++a )
  {
    for( std::uint8_t b = 0; b < 2; ++b )
    {
      auto code = create_code_segment( opcode::equals_b, opcode::exit );
      perseus::stack stack;
      stack.push< std::uint8_t >( a );
      stack.push< std::uint8_t >( b );
      perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
      BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), a == b );
      BOOST_CHECK_EQUAL( result.size(), 0 );
    }
  }
}

BOOST_AUTO_TEST_CASE( boolean_not_equals )
{
  for( std::uint8_t a = 0; a < 2; ++a )
  {
    for( std::uint8_t b = 0; b < 2; ++b )
    {
      auto code = create_code_segment( opcode::not_equals_b, opcode::exit );
      perseus::stack stack;
      stack.push< std::uint8_t >( a );
      stack.push< std::uint8_t >( b );
      perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
      BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), a != b );
      BOOST_CHECK_EQUAL( result.size(), 0 );
    }
  }
}

BOOST_AUTO_TEST_CASE( boolean_negate )
{
  for( std::uint8_t a = 0; a < 2; ++a )
  {
    auto code = create_code_segment( opcode::negate_b, opcode::exit );
    perseus::stack stack;
    stack.push< std::uint8_t >( a );
    perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
    BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), !a );
    BOOST_CHECK_EQUAL( result.size(), 0 );
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( int32_arithmetic )

BOOST_AUTO_TEST_CASE( int32_add )
{
  auto code = create_code_segment( opcode::add_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 4250 );
  stack.push< std::int32_t >( -8 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 4242 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_subtract )
{
  auto code = create_code_segment( opcode::subtract_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 1008 );
  stack.push< std::int32_t >( 1050 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), -42 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_multiply )
{
  auto code = create_code_segment( opcode::multiply_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 6 );
  stack.push< std::int32_t >( 7 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 42 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_divide_no_remainder )
{
  auto code = create_code_segment( opcode::divide_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 42 );
  stack.push< std::int32_t >( 7 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 6 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_divide_round_down )
{
  BOOST_TEST_MESSAGE( "48/7 is closer to 7 than 6, but integer division is supposed to round down." );
  auto code = create_code_segment( opcode::divide_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 48 );
  stack.push< std::int32_t >( 7 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 6 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_modulo )
{
  auto code = create_code_segment( opcode::modulo_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 47 );
  stack.push< std::int32_t >( 7 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 5 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_less_than_true )
{
  auto code = create_code_segment( opcode::less_than_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 1 );
  stack.push< std::int32_t >( 2 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), true );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_less_than_false )
{
  auto code = create_code_segment( opcode::less_than_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 2 );
  stack.push< std::int32_t >( 2 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), false );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_less_than_or_equals_true )
{
  auto code = create_code_segment( opcode::less_than_or_equals_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 2 );
  stack.push< std::int32_t >( 2 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), true );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_less_than_or_equals_false )
{
  auto code = create_code_segment( opcode::less_than_or_equals_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( 3 );
  stack.push< std::int32_t >( 2 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( static_cast< bool >( result.pop< std::uint8_t >() ), false );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( int32_negate )
{
  auto code = create_code_segment( opcode::negate_i32, opcode::exit );
  perseus::stack stack;
  stack.push< std::int32_t >( -42 );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::int32_t >(), 42 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
