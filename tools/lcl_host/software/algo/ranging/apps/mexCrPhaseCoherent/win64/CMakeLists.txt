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
include (${PROJECT_LIBS}/crPhaseCoherent/cmake_lib_crPhaseCoherent.inc)
include (${PROJECT_LIBS}/mexsupport/cmake_lib_mexsupport.inc)

# optional components 

# ---------------------------------------------------------
# mexCrPhaseCoherent
# ---------------------------------------------------------

add_compile_definitions(WIN_EXPORT NDEBUG ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating mexCrPhaseCoherent")
add_library(mexCrPhaseCoherent SHARED
	${SOURCES}
	../src/mexCrPhaseCoherent.cpp
)

target_include_directories(mexCrPhaseCoherent PRIVATE ../src ${MATLAB_INCLUDES})
target_compile_definitions(mexCrPhaseCoherent PRIVATE MX_COMPAT_32 MATLAB_MEX_FILE NDEBUG)
target_compile_definitions(mexCrPhaseCoherent PRIVATE __PRETTY_FUNCTION__="mexCrPhaseCoherent")
foreach(var ${MEX_LIBRARIES})
	unset(MEX_LIB CACHE)
	find_library(MEX_LIB ${var} ${MATLAB_LIB_PATH})
	target_link_libraries(mexCrPhaseCoherent ${MEX_LIB})
endforeach(var)
set_target_properties(mexCrPhaseCoherent PROPERTIES SUFFIX ".mexw64" LINK_FLAGS /export:mexFunction
)
