#include "vm/code_segment.hpp"
#include "vm/instruction_pointer.hpp"

#include <boost/test/unit_test.hpp>

// Tests for code_segment writing (especially opcodes, which are encoded) and instruction_pointer reading (including past the end)

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( instruction_pointer )

struct instruction_pointer_fixture
{
  instruction_pointer_fixture() : pointer( segment ) {}
  perseus::detail::code_segment segment;
  perseus::detail::instruction_pointer pointer;
};

BOOST_FIXTURE_TEST_CASE( moving, instruction_pointer_fixture )
{
  using perseus::detail::instruction_pointer;
  segment.push< char >( 'a' );
  segment.push< char >( 'b' );
  segment.push< char >( 'c' );

  BOOST_CHECK_EQUAL( 0, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_CHECK_EQUAL( 'a', pointer.read< char >() );
  BOOST_CHECK_EQUAL( 1, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_CHECK_EQUAL( 'b', pointer.read< char >() );
  BOOST_CHECK_EQUAL( 2, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_CHECK_EQUAL( 'c', pointer.read< char >() );
  BOOST_CHECK_EQUAL( 3, static_cast< instruction_pointer::value_type >( pointer ) );
  // check this with a copy so the original remains unchanged
  BOOST_CHECK_THROW( instruction_pointer( pointer ).read< char >(), perseus::code_segmentation_fault );
  BOOST_CHECK_EQUAL( 3, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_REQUIRE_THROW( pointer += 1, perseus::code_segmentation_fault );
  BOOST_CHECK_EQUAL( 3, static_cast< instruction_pointer::value_type >( pointer ) );
  pointer += -3;
  BOOST_CHECK_EQUAL( 0, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_CHECK_EQUAL( 'a', pointer.read< char >() );
  BOOST_CHECK_EQUAL( 1, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_REQUIRE_THROW( pointer += -2, perseus::code_segmentation_fault );
  BOOST_CHECK_EQUAL( 1, static_cast< instruction_pointer::value_type >( pointer ) );
  pointer += 1;
  BOOST_CHECK_EQUAL( 2, static_cast< instruction_pointer::value_type >( pointer ) );
  BOOST_CHECK_EQUAL( 'c', pointer.read< char >() );
  BOOST_CHECK_EQUAL( 3, static_cast< instruction_pointer::value_type >( pointer ) );
}

BOOST_FIXTURE_TEST_CASE( singlebyte_encoding, instruction_pointer_fixture )
{
  using perseus::detail::opcode;
  opcode code = static_cast< opcode >( 42 ); // something that fits into a single byte
  segment.push< opcode >( code );

  BOOST_CHECK_EQUAL( code, pointer.read< opcode >() );
  BOOST_CHECK_THROW( pointer.read< char >(), perseus::code_segmentation_fault );
}

BOOST_FIXTURE_TEST_CASE( multibyte_encoding, instruction_pointer_fixture )
{
  using perseus::detail::opcode;
  opcode code = static_cast< opcode >( 12345 ); // something that would not fit into a single byte
  segment.push< opcode >( code );
  segment.push< unsigned int >( 42 );

  BOOST_CHECK_EQUAL( code, pointer.read< opcode >() );
  BOOST_CHECK_EQUAL( 42u, pointer.read< unsigned int >() );
  BOOST_CHECK_THROW( pointer.read< char >(), perseus::code_segmentation_fault );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
