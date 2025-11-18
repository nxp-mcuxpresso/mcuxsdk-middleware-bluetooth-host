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
include (${PROJECT_LIBS}/common/cmake_lib_common.inc)
include (${PROJECT_LIBS}/crNonPhaseCoherent/cmake_lib_crNonPhaseCoherent.inc OPTIONAL)
include (${PROJECT_LIBS}/mexsupport/cmake_lib_mexsupport.inc OPTIONAL)

# ---------------------------------------------------------
# mexCrNonPhaseCoherent
# ---------------------------------------------------------

add_compile_definitions(ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating mexCrNonPhaseCoherent")
add_library(mexCrNonPhaseCoherent SHARED
	${SOURCES}
	../src/mexCrNonPhaseCoherent.cpp
)

target_compile_definitions(mexCrNonPhaseCoherent PRIVATE ${MEX_COMPILE_DEFINITIONS} __PRETTY_FUNCTION__="mexCrNonPhaseCoherent")
target_include_directories(mexCrNonPhaseCoherent PRIVATE ../src ${MATLAB_INCLUDES})

foreach(var ${MEX_LIBRARIES})
	unset(MEX_LIB CACHE)
	find_library(MEX_LIB ${var} ${MATLAB_LIB_PATH})
	target_link_libraries(mexCrNonPhaseCoherent ${MEX_LIB})
endforeach(var)
set_target_properties(mexCrNonPhaseCoherent PROPERTIES 
	PREFIX ""
	SUFFIX "${MEX_SUFFIX}"
	LINK_FLAGS ${MEX_LINK_FLAGS}
)


# end
