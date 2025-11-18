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
include (${PROJECT_LIBS}/music/cmake_lib_music.inc OPTIONAL)
include (${PROJECT_LIBS}/h2cov/cmake_lib_h2cov.inc OPTIONAL)
include (${PROJECT_LIBS}/evd/cmake_lib_evd.inc OPTIONAL)

# ---------------------------------------------------------
# Include Unity for testing
# ---------------------------------------------------------
include (${PROJECT_DIR}/thirdparty/cmake_unity.inc)

# ---------------------------------------------------------
# Build executable
# ---------------------------------------------------------

add_compile_definitions(ARM_MATH_MATRIX_CHECK ARM_MATH_CM4 RANGE_CHECK=1 USE_MEMTRACE=0 USE_STOPWATCH=0 STOPWATCH_MAXNESTING=1)

message(STATUS "creating musictest")
add_executable(musictest
	${SOURCES}
	../src/main.cpp
	../src/test_execute.cpp
	../src/test_music_ranging.cpp
	../src/test_music_ranging_p054.cpp
	../src/test_music_real.cpp 
)
target_include_directories(musictest PRIVATE ../src)
set_target_properties(musictest PROPERTIES 
	LINK_FLAGS "${PUBLIC_LINK_FLAGS}"
)

# end
