#include "write_code_segment.hpp"

#include "vm/processor.hpp"

#include <boost/test/unit_test.hpp>

using perseus::detail::processor;
using perseus::detail::opcode;
using perseus::invalid_opcode;
using perseus::code_segmentation_fault;

BOOST_AUTO_TEST_SUITE( vm )

BOOST_AUTO_TEST_SUITE( execution )

BOOST_AUTO_TEST_CASE( exit_test )
{
  processor( create_code_segment( opcode::exit ) ).execute();
}

BOOST_AUTO_TEST_CASE( noop_test )
{
  processor( create_code_segment( opcode::no_operation, opcode::exit ) ).execute();
}

BOOST_AUTO_TEST_CASE( out_of_bounds_test )
{
  BOOST_CHECK_THROW( processor( create_code_segment( opcode::no_operation ) ).execute(), code_segmentation_fault );
}

BOOST_AUTO_TEST_CASE( invalid_opcode_test )
{
  BOOST_CHECK_THROW( processor( create_code_segment( opcode::opcode_end ) ).execute(), invalid_opcode );
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
