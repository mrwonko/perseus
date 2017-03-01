cmake_minimum_required(VERSION 3.1)

project( "perseus" )

# enable C++14 support
set(CMAKE_CXX_STANDARD 14)

set( Boost_USE_STATIC_LIBS ON )
# add_definitions( "-DBOOST_TEST_DYN_LINK" )
find_package( Boost COMPONENTS unit_test_framework REQUIRED )

set( SHARED_FILES
    "src/shared/opcodes.cpp" "include/shared/opcodes.hpp"
    "include/shared/documentation.hpp"
    "include/shared/optional_apply.hpp"
)

set( COMPILER_FILES
    "src/compiler/token_ids.cpp" "include/compiler/token_ids.hpp"
    "src/compiler/token_definitions.cpp" "include/compiler/token_definitions.hpp"
    "src/compiler/grammar.cpp" "include/compiler/grammar.hpp"
    "src/compiler/conversions.cpp" "include/compiler/conversions.hpp"
    "include/compiler/exceptions.hpp"
    "include/compiler/ast.hpp"
    "include/compiler/ast_common.inl"
    "include/compiler/ast_common_fwd.inl"
    "include/compiler/ast_adapted.hpp"
    "src/compiler/iterators.cpp" "include/compiler/iterators.hpp"
    "src/compiler/compiler.cpp" "include/compiler/compiler.hpp"
    "include/compiler/steps.hpp"
    "src/compiler/steps/extract_functions.cpp"
    "src/compiler/steps/simplify_and_annotate.cpp"
    "src/compiler/steps/generate_code.cpp"
    "src/compiler/types.cpp" "include/compiler/types.hpp"
    "src/compiler/function_manager.cpp" "include/compiler/function_manager.hpp"
)

set( VM_FILES
    "include/vm/exceptions.hpp"
    "src/vm/processor.cpp" "include/vm/processor.hpp"
    "src/vm/code_segment.cpp" "include/vm/code_segment.hpp"
    "src/vm/instruction_pointer.cpp" "include/vm/instruction_pointer.hpp"
    "src/vm/stack.cpp" "include/vm/stack.hpp"
    "src/vm/coroutine.cpp" "include/vm/coroutine.hpp"
    "src/vm/coroutine_manager.cpp" "include/vm/coroutine_manager.hpp"
)

set( UTIL_FILES
    "include/util/u32string_ostream.hpp"
    )

set( TEST_FILES
    "test/test_init.cpp"
	"test/execution/arithmetic.cpp"
    "test/execution/basic.cpp"
    "test/execution/coroutines.cpp"
	"test/execution/jump_call.cpp"
	"test/execution/load_store.cpp"
    "test/execution/stack.cpp"
    "test/write_code_segment.hpp"
    "test/instruction_pointer.cpp"
    "test/stack.cpp"
    "test/cpp.cpp"
    "test/compiler/iterators.cpp"
    "test/compiler/lexer.cpp"
    "test/compiler/parser.cpp"
    "test/compiler/literals.cpp"
    "test/compiler/function_manager.cpp"
)

source_group( "src" REGULAR_EXPRESSION "src/.*" )
source_group( "src\\vm" REGULAR_EXPRESSION "src/vm/.*" )
source_group( "src\\shared" REGULAR_EXPRESSION "src/shared/.*" )
source_group( "src\\compiler" REGULAR_EXPRESSION "src/compiler/.*" )
source_group( "src\\compiler\\steps" REGULAR_EXPRESSION "src/compiler/steps/.*" )

source_group( "include" REGULAR_EXPRESSION "include/.*" )
source_group( "include\\vm" REGULAR_EXPRESSION "include/vm/.*" )
source_group( "include\\shared" REGULAR_EXPRESSION "include/shared/.*" )
source_group( "include\\compiler" REGULAR_EXPRESSION "include/compiler/.*" )

source_group( "test" REGULAR_EXPRESSION "test/.*" )
source_group( "test\\execution" REGULAR_EXPRESSION "test/execution/.*" )
source_group( "test\\compiler" REGULAR_EXPRESSION "test/compiler/.*" )

# If doxygen is available, add a target for it
find_package( Doxygen )
if( DOXYGEN_FOUND )
    configure_file( "Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY )
    add_custom_target( doc ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
else( DOXYGEN_FOUND )
    message( "Could not find Doxygen, no doc target created." )
endif( DOXYGEN_FOUND )

include_directories( "include" "${Boost_INCLUDE_DIRS}" )
link_directories(${Boost_LIBRARY_DIRS})

add_library( perseus
    ${VM_FILES}
	${COMPILER_FILES}
    ${SHARED_FILES}
)

add_executable( testsuite
    ${TEST_FILES}
    ${UTIL_FILES}
)

add_executable( perseus_print
    "executable/print/main.cpp"
    "executable/print/print_clean_ast.hpp"
    "executable/print/print_parser_ast.hpp"
    "executable/print/print_ast.hpp"
    "executable/print/print_ast.inl"
    ${UTIL_FILES}
)

add_executable( runperseus
    "executable/run.cpp"
    ${UTIL_FILES}
)

target_link_libraries( testsuite ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} perseus )
target_link_libraries( runperseus perseus )
target_link_libraries( perseus_print perseus )
