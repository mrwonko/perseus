#include "vm/code_segment.hpp"
#include "vm/instruction_pointer.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( opcode_encoding )

struct opcode_encoding_fixture
{
  opcode_encoding_fixture() : pointer( segment ) {}
  perseus::detail::code_segment segment;
  perseus::detail::instruction_pointer pointer;
};

BOOST_FIXTURE_TEST_CASE( singlebyte_test, opcode_encoding_fixture )
{
  using perseus::detail::opcode;
  opcode code = static_cast< opcode >( 42 ); // something that fits into a single byte
  segment.push< opcode >( code );

  BOOST_CHECK_EQUAL( code, pointer.read< opcode >() );
  BOOST_CHECK_THROW( pointer.read< char >(), perseus::code_segmentation_fault );
}

BOOST_FIXTURE_TEST_CASE( multibyte_test, opcode_encoding_fixture )
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
