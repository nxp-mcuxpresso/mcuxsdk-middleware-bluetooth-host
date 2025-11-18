#
# Append source files and flags for lib h2cov
#
set(SRC_H2COV_DIR ${PROJECT_DIR}/libs/h2cov)

include_directories(${SRC_H2COV_DIR}/include )

set(SOURCES_H2COV
	${SRC_H2COV_DIR}/src/h2cov.cpp
)

add_compile_definitions(COMPONENT_H2COV)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_H2COV}
)
# end lib h2cov
