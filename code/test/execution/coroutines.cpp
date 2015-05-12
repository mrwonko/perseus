#include "../write_code_segment.hpp"

#include "vm/processor.hpp"
#include "vm/stack.hpp"
#include "vm/exceptions.hpp"

#include <utility>
#include <cstdint>

#include <boost/test/unit_test.hpp>

/**
@file

Coroutine checks

@todo Write some actual complex tests
*/

using perseus::detail::processor;
using perseus::detail::opcode;
using perseus::stack;
using address = perseus::detail::instruction_pointer::value_type;
using identifier = perseus::detail::coroutine::identifier;

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_SUITE( coroutines )

BOOST_AUTO_TEST_CASE( root_yield )
{
  BOOST_MESSAGE( "The root coroutine must not yield." );
  auto code = create_code_segment( opcode::yield, std::uint32_t( 0 ) );
  processor proc( std::move( code ) );
  BOOST_CHECK_THROW( proc.execute(), perseus::no_coroutine );
}

BOOST_AUTO_TEST_CASE( root_return )
{
  BOOST_MESSAGE( "The root coroutine must not return." );
  auto code = create_code_segment( opcode::coroutine_return, std::uint32_t( 0 ) );
  processor proc( std::move( code ) );
  BOOST_CHECK_THROW( proc.execute(), perseus::no_coroutine );
}

static void leaky_coroutine()
{
}

BOOST_AUTO_TEST_CASE( create_delete )
{
  BOOST_MESSAGE( "creating & deleting an unused coroutine" );
  address del_adr;
  auto code = create_code_segment(
    opcode::absolute_coroutine, std::uint32_t( 0 ),
    opcode::exit,

    get_current_address( del_adr ),
    opcode::delete_coroutine,
    opcode::exit
    );
  processor proc( std::move( code ) );
  stack result = proc.execute();
  BOOST_CHECK_EQUAL( result.size(), sizeof( identifier ) );
  BOOST_CHECK( proc.has_coroutines() );
  proc.execute( del_adr, std::move( result ) );
  BOOST_CHECK( !proc.has_coroutines() );
}

// TODO invalid identifier

// TODO resume/yield/return

// TODO resume_pushing_everything

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
