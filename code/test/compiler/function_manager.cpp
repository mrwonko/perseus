#include "compiler/function_manager.hpp"

#include <boost/test/unit_test.hpp>

using namespace perseus::detail;

typedef function_manager::function_map::const_iterator iterator;

BOOST_AUTO_TEST_SUITE( compiler )

BOOST_AUTO_TEST_SUITE( function_manager_tests )

using namespace std::string_literals;

BOOST_AUTO_TEST_CASE( register_functions )
{
  using namespace perseus::detail;
  function_manager man;
  {
    iterator add;
    BOOST_REQUIRE( man.register_function( function_signature{ "add"s, { type_id::i32, type_id::i32 } }, function_info{ type_id::i32 }, add ) );

    iterator subtract;
    BOOST_REQUIRE( man.register_function( function_signature{ "subtract"s,{ type_id::i32, type_id::i32 } }, function_info{ type_id::i32 }, subtract ) );

    BOOST_CHECK_EQUAL( add->first.name, "add"s );
    BOOST_CHECK_EQUAL( subtract->first.name, "subtract"s );
  }
}

BOOST_AUTO_TEST_CASE( functions_lookup )
{
  function_manager man;
  {
    function_signature add_signature{ "add"s,{ type_id::i32, type_id::i32 } };
    iterator add;
    BOOST_REQUIRE( man.register_function( function_signature{ add_signature }, function_info{ type_id::i32 }, add ) );

    iterator subtract;
    BOOST_REQUIRE( man.register_function( function_signature{ "subtract"s,{ type_id::i32, type_id::i32 } }, function_info{ type_id::i32 }, subtract ) );

    iterator found_add;
    BOOST_REQUIRE( man.get_function( add_signature, found_add ) );
    BOOST_CHECK_EQUAL( found_add->first.name, "add"s );
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
