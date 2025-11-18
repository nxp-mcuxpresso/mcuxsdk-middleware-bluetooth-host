
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_program(PYTHON_EXE python)
get_filename_component(PYTHON_DIR ${PYTHON_EXE} DIRECTORY)
find_program(MATLAB_EXE matlab)
get_filename_component(MATLAB_BIN ${MATLAB_EXE} DIRECTORY)
get_filename_component(MATLAB_DIR ${MATLAB_BIN} DIRECTORY)

set(PYTHON_LIBRARY python36)
set(PYTHON_LIB_PATH ${PYTHON_DIR}/libs)
set(PYTHON_INCLUDES ${PYTHON_DIR}/include)
message(STATUS "PYTHON_INCLUDES = ${PYTHON_INCLUDES}")

set(MEX_LIBRARIES libmex libmat libmwlapack libmwblas libmx)
set(MATLAB_LIB_PATH "${MATLAB_DIR}/extern/lib/win64/microsoft")
set(MATLAB_INCLUDES "${MATLAB_DIR}/extern/include" "${MATLAB_DIR}/simulink/include")
message(STATUS "MATLAB_LIB_PATH = ${MATLAB_LIB_PATH}")
message(STATUS "MATLAB_INCLUDES = ${MATLAB_INCLUDES}")

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
# See https://cmake.org/cmake/help/v3.4/variable/CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS.html
# See https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_compile_options(/EHsc /W2 /Os /wd4003 /wd4305)
add_compile_options(/fp:strict)
# Set the DLLEXPORT variable to export symbols
