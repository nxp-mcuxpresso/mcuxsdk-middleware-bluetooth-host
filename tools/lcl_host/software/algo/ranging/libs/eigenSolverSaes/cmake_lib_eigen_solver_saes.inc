#
# Append source files and flags for lib eigen solver
#
set(SRC_EIGEN_SOLVER_SAES_DIR ${PROJECT_DIR}/libs/eigenSolverSaes)

include_directories(${SRC_EIGEN_SOLVER_SAES_DIR}/include )

set(SOURCES_EIGEN_SOLVER_SAES
	${SRC_EIGEN_SOLVER_SAES_DIR}/src/EigenSolverSaes.cpp
)

add_compile_definitions(COMPONENT_EIGEN_SOLVER_SAES)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_EIGEN_SOLVER_SAES}
)
# end lib eigen solver
