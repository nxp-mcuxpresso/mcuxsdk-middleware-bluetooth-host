#
# Append source files and flags for Eigen Library
#

include_directories(${PROJECT_DIR}/thirdparty/Eigen/eigen-3.3.7 )

# append to sources ( Nothing )

add_compile_definitions(COMPONENT_EIGEN)
add_compile_definitions(EIGEN_MPL2_ONLY)

# end Eigen
