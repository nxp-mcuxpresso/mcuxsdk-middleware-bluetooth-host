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

# ---------------------------------------------------------
# Include Ranging libraries
# ---------------------------------------------------------

# mandatory components 
include (${PROJECT_LIBS}/common/cmake_lib_common_win64.inc)
include (${PROJECT_LIBS}/music/cmake_lib_music.inc)
include (${PROJECT_LIBS}/h2cov/cmake_lib_h2cov.inc)
include (${PROJECT_LIBS}/mexsupport/cmake_lib_mexsupport.inc)

# optional components 
include (${PROJECT_LIBS}/evd/cmake_lib_evd.inc OPTIONAL)
include (${PROJECT_LIBS}/eigenSolverSaes/cmake_lib_eigen_solver_saes.inc OPTIONAL)
include (${PROJECT_DIR}/thirdparty/cmake_eigen.inc OPTIONAL)

# ---------------------------------------------------------
# mexMusic
# ---------------------------------------------------------

add_compile_definitions(WIN_EXPORT NDEBUG ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating mexMusic")
add_library(mexMusic SHARED
	${SOURCES}
	../src/mexMusic.cpp
)

target_include_directories(mexMusic PRIVATE ../src ${MATLAB_INCLUDES})
target_compile_definitions(mexMusic PRIVATE MX_COMPAT_32 MATLAB_MEX_FILE NDEBUG)
target_compile_definitions(mexMusic PRIVATE __PRETTY_FUNCTION__="mexMusic")
foreach(var ${MEX_LIBRARIES})
	unset(MEX_LIB CACHE)
	find_library(MEX_LIB ${var} ${MATLAB_LIB_PATH})
	target_link_libraries(mexMusic ${MEX_LIB})
endforeach(var)
set_target_properties(mexMusic PROPERTIES SUFFIX ".mexw64" LINK_FLAGS /export:mexFunction
)
