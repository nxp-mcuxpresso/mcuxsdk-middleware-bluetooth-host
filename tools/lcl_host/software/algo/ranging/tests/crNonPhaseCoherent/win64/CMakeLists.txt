# ---------------------------------------------------------
# CMAKE build script for win64 binaries
# ---------------------------------------------------------

cmake_minimum_required(VERSION 3.12)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_GENERATOR_PLATFORM x64)

project(ranging)

set(PROJECT_DIR ../../..)
set(PROJECT_LIBS ${PROJECT_DIR}/libs)
set(SRC_DIR ${PROJECT_DIR})

include_directories(${PROJECT_LIBS})

include (${PROJECT_DIR}/cmake_win64.inc)

# mandatory components 
include (${PROJECT_LIBS}/common/cmake_lib_common_win64.inc)
include (${PROJECT_LIBS}/crNonPhaseCoherent/cmake_lib_crNonPhaseCoherent.inc)

# optional components 

# test support
include (${PROJECT_LIBS}/testsupport/cmake_lib_testsupport.inc)
include (${PROJECT_DIR}/thirdparty/cmake_unity.inc)

# ---------------------------------------------------------
# testCrNonPhaseCoherent
# ---------------------------------------------------------

add_compile_definitions(WIN_EXPORT NDEBUG ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating testCrNonPhaseCoherent")
add_executable(testCrNonPhaseCoherent
	${SOURCES}
	../src/main.cpp
	../src/test_execute.cpp
	../src/test_reconstructNonPhaseCoherent.cpp
)
target_include_directories(testCrNonPhaseCoherent PRIVATE ../src)
set_target_properties(testCrNonPhaseCoherent PROPERTIES LINK_FLAGS "${PUBLIC_LINK_FLAGS}")

