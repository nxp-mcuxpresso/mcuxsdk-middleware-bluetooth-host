cmake_minimum_required(VERSION 3.12)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CXX_STANDARD 11)

project(ranging)

set(PROJECT_DIR ../../..)
set(PROJECT_LIBS ${PROJECT_DIR}/libs)

include (${PROJECT_DIR}/cmake_linux.inc)

include_directories(${PROJECT_LIBS})

# ---------------------------------------------------------
# Include Ranging libraries
# ---------------------------------------------------------

# mandatory components 
include (${PROJECT_LIBS}/common/cmake_lib_common.inc)
include (${PROJECT_LIBS}/music/cmake_lib_music.inc)
include (${PROJECT_LIBS}/h2cov/cmake_lib_h2cov.inc)
include (${PROJECT_LIBS}/mexsupport/cmake_lib_mexsupport.inc)

# optional components 
include (${PROJECT_LIBS}/evd/cmake_lib_evd.inc OPTIONAL)
include (${PROJECT_LIBS}/eigenSolverSaes/cmake_lib_eigen_solver_saes.inc OPTIONAL)
include (${PROJECT_DIR}/thirdparty/cmake_eigen.inc OPTIONAL)

# ---------------------------------------------------------
# Include Eigen Library
# ---------------------------------------------------------
include (${PROJECT_DIR}/thirdparty/cmake_eigen.inc OPTIONAL)

# ---------------------------------------------------------
# mexMusic
# ---------------------------------------------------------

add_compile_definitions(ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_MEMTRACE=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating mexMusic")
add_library(mexMusic SHARED
	${SOURCES}
	../src/mexMusic.cpp
)

target_compile_definitions(mexMusic PRIVATE ${MEX_COMPILE_DEFINITIONS} __PRETTY_FUNCTION__="mexMusic")
target_include_directories(mexMusic PRIVATE ../src ${MATLAB_INCLUDES})

foreach(var ${MEX_LIBRARIES})
	unset(MEX_LIB CACHE)
	find_library(MEX_LIB ${var} ${MATLAB_LIB_PATH})
	target_link_libraries(mexMusic ${MEX_LIB})
endforeach(var)
set_target_properties(mexMusic PROPERTIES 
	PREFIX ""
	SUFFIX "${MEX_SUFFIX}"
	LINK_FLAGS ${MEX_LINK_FLAGS}
)


# end
