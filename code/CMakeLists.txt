cmake_minimum_required(VERSION 2.6)

project( "perseus" )

# enable C++14 support
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif(CMAKE_COMPILER_IS_GNUCXX)

set( Boost_USE_STATIC_LIBS ON )
# add_definitions( "-DBOOST_TEST_DYN_LINK" )
find_package( Boost COMPONENTS unit_test_framework REQUIRED )

set( EXTERNAL_FILES
    "lib/utf8.h"
    "lib/utf8/checked.h"
    "lib/utf8/unchecked.h"
    "lib/utf8/core.h"
    )

set( SHARED_FILES
    "src/shared/opcodes.cpp" "include/shared/opcodes.hpp"
    "include/shared/documentation.hpp"
)

set( COMPILER_FILES
    "include/compiler/iterators.hpp"
    "src/compiler/compile.cpp" "include/compiler/compile.hpp"
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
    "test/compiler/iterators.cpp"
)

source_group( "src" REGULAR_EXPRESSION "src/.*" )
source_group( "src\\vm" REGULAR_EXPRESSION "src/vm/.*" )
source_group( "src\\shared" REGULAR_EXPRESSION "src/shared/.*" )
source_group( "src\\compiler" REGULAR_EXPRESSION "src/compiler/.*" )

source_group( "include" REGULAR_EXPRESSION "include/.*" )
source_group( "include\\vm" REGULAR_EXPRESSION "include/vm/.*" )
source_group( "include\\shared" REGULAR_EXPRESSION "include/shared/.*" )
source_group( "include\\compiler" REGULAR_EXPRESSION "include/compiler/.*" )

source_group( "test" REGULAR_EXPRESSION "test/.*" )
source_group( "test\\execution" REGULAR_EXPRESSION "test/execution/.*" )
source_group( "test\\compiler" REGULAR_EXPRESSION "test/compiler/.*" )

source_group( "lib" REGULAR_EXPRESSION "lib/.*" )
source_group( "lib\\utf8" REGULAR_EXPRESSION "lib/utf8/.*" )

# If doxygen is available, add a target for it
find_package( Doxygen )
if( DOXYGEN_FOUND )
    configure_file( "Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY )
    add_custom_target( doc ALL ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
else( DOXYGEN_FOUND )
    message( "Could not find Doxygen, no doc target created." )
endif( DOXYGEN_FOUND )

include_directories( "include" "lib" "${Boost_INCLUDE_DIRS}" )
link_directories(${Boost_LIBRARY_DIRS})

add_library( perseus
    ${VM_FILES}
	${COMPILER_FILES}
    ${SHARED_FILES}
    ${EXTERNAL_FILES}
)

add_executable( testsuite
    ${TEST_FILES}
)

target_link_libraries( testsuite ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} perseus )
