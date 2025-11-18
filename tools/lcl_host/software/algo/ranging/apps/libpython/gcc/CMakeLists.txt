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

# optional components 
include (${PROJECT_LIBS}/evd/cmake_lib_evd.inc OPTIONAL)
include (${PROJECT_LIBS}/eigenSolverSaes/cmake_lib_eigen_solver_saes.inc OPTIONAL)
include (${PROJECT_DIR}/thirdparty/cmake_eigen.inc OPTIONAL)
include (${PROJECT_LIBS}/crNonPhaseCoherent/cmake_lib_crNonPhaseCoherent.inc OPTIONAL)
include (${PROJECT_LIBS}/crPhaseCoherent/cmake_lib_crPhaseCoherent.inc OPTIONAL)

# ---------------------------------------------------------
# _ranging.pyd
# ---------------------------------------------------------

add_compile_definitions(ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating _ranging.so")
add_library(_ranging SHARED
	${SOURCES}
	../src/_ranging_module.cpp
	../src/PythonWrapper.cpp
)
target_include_directories(_ranging PRIVATE ${PYTHON_INCLUDES} ../src)
find_library(PYTHON_LIB ${PYTHON_LIBRARY} ${PYTHON_LIB_PATH})
message(STATUS "PYTHON_LIB = ${PYTHON_LIB}")
# target_link_libraries(_ranging "${PYTHON_LIB}")

set_target_properties(_ranging PROPERTIES 
	PREFIX ""
	SUFFIX "${LIB_SUFFIX}"
	LINK_FLAGS "${PUBLIC_LINK_FLAGS}"
)

# end
