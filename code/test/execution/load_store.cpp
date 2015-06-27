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

BOOST_AUTO_TEST_SUITE( load_and_store )

BOOST_AUTO_TEST_CASE( stack_size )
{
  auto code = create_code_segment( opcode::push_stack_size, opcode::push_stack_size, opcode::exit );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), sizeof( std::uint32_t ) );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0 );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( relative_load )
{
  perseus::stack stack;
  stack.push< std::uint32_t >( 0xDEADBEEF );
  stack.push< std::uint32_t >( 0xBAADF00D );
  stack.push< std::uint32_t >( 0xC0DED00D );
  auto code = create_code_segment(
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -4 ),
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -12 ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xBAADF00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xC0DED00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xC0DED00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xBAADF00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( absolute_load_current )
{
  perseus::stack stack;
  stack.push< std::uint32_t >( 0xDEADBEEF );
  stack.push< std::uint32_t >( 0xBAADF00D );
  stack.push< std::uint32_t >( 0xC0DED00D );
  stack.push< std::uint32_t >( 0 ); // address to load
  auto code = create_code_segment(
    opcode::absolute_load_current_stack, std::uint32_t( 8 ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute( 0, std::move( stack ) );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xBAADF00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xC0DED00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xBAADF00D );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( relative_store )
{
  auto code = create_code_segment(
    opcode::reserve, std::uint32_t( 8 ),
    opcode::push_32, std::uint32_t( 0xDEADBEEF ),
    opcode::relative_store_stack, std::uint32_t( 4 ), std::int32_t( -12 ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0 );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( absolute_store_current )
{
  auto code = create_code_segment(
    opcode::reserve, std::uint32_t( 8 ),
    opcode::push_32, std::uint32_t( 0xDEADBEEF ),
    opcode::push_32, std::uint32_t( 0 ), // target address
    opcode::absolute_store_current_stack, std::uint32_t( 4 ),
    opcode::exit
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0 );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xDEADBEEF );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}

BOOST_AUTO_TEST_CASE( absolute_load_store )
{
  // the following code is roughly equivalent to this:
  /*
  void main()
  {
    mut u32 local1 = 0x01234567
    mut u32 local2 = 0x89ABCDEF
    mut ( u32, u32 ) result
    {
      mut u32 coroutine_id = coroutine.new( coroutine.start )
      result = coroutine.resume( coroutine_id, &local1 )
      coroutine.delete( coroutine_id )
    }
  }

  ( u32, u32 ) coroutine_start( mut u32* locals )
  {
    ( u32, u32 ) result = ( locals[ 0 ], locals[ 1 ] )
    locals[ 0 ] = 0xFFFFFFFF
    return result
  }
  */
  auto code = create_code_segment(
    //    load data onto stack
    // mut u32 local1 = 0x01234567
    opcode::push_32, std::uint32_t( 0x01234567 ),
    // mut u32 local2 = 0x89ABCDEF
    opcode::push_32, std::uint32_t( 0x89ABCDEF ),

    // create coroutine; stack is now [local1] [local2] [&coroutine]
    opcode::absolute_coroutine, label_reference( "coroutine_start" ),

    // target stack & address; stack is now [local1] [local2] [&coroutine] [&main] [&local1]
    opcode::push_coroutine_identifier,
    opcode::push_32, std::uint32_t( 0 ),

    // coroutine to resume
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -12 ),

    // launch coroutine, popping target stack & address as parameters; stack is now [local1] [local2] [&coroutine] [8 byte coroutine return values]
    opcode::resume_coroutine, std::uint32_t( 8 ),

    // load coroutine id... - stack is now [local1] [local2] [&coroutine] [8 byte coroutine return values] [&coroutine]
    opcode::relative_load_stack, std::uint32_t( 4 ), std::int32_t( -12 ),
    // ... and delete it. stack is now [local1] [local2] [&coroutine] [8 byte coroutine return values]
    opcode::delete_coroutine,

    // overwrite stored coroutine id; stack is now [local1] [local2] [8 byte coroutine return values] [last 4 byte of coroutine return value]
    opcode::relative_store_stack, std::uint32_t( 8 ), std::int32_t( -12 ),
    // pop those duplicate 4 bytes, stack is now [local1] [local2] [8 byte coroutine return values]
    opcode::pop, std::uint32_t( 4 ),

    opcode::exit,

    label( "coroutine_start" ),
    // initial stack: [&main] [&main.local1]
    // duplicate those so we can read & write
    opcode::relative_load_stack, std::uint32_t( 8 ), std::int32_t( -8 ),
    // load; stack: [&main] [&main.local1] [main.local1] [main.local2]
    opcode::absolute_load_stack, std::uint32_t( 8 ),
    // put some data to store onto the stack; stack: [&main] [&main.local1] [main.local1] [main.local2] 0xFFFFFFFF
    opcode::push_32, std::uint32_t( 0xFFFFFFFF ),
    // copy address to top; stack: [&main] [&main.local1] [main.local1] [main.local2] 0xFFFFFFFF [&main] [&main.local1]
    opcode::relative_load_stack, std::uint32_t( 8 ), std::int32_t( -20 ),
    // write into main stack; stack: [&main] [&main.local1] [main.local1] [main.local2] 0xFFFFFFFF
    opcode::absolute_store_stack, std::uint32_t( 4 ),
    // discard written data; stack: [&main] [&main.local1] [main.local1] [main.local2]
    opcode::pop, std::uint32_t( 4 ),
    // return the top 8 bytes from stack to parent coroutine (bottom 8 get discarded)
    opcode::coroutine_return, std::uint32_t( 8 )
    );
  perseus::stack result = processor( std::move( code ) ).execute();
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0x89ABCDEF );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0x01234567 );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0x89ABCDEF );
  BOOST_CHECK_EQUAL( result.pop< std::uint32_t >(), 0xFFFFFFFF );
  BOOST_CHECK_EQUAL( result.size(), 0 );
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
