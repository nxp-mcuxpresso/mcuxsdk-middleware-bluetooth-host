#
# Append source files and flags for lib mexsupport
#
set(SRC_MEXSUPPORT_DIR ${PROJECT_DIR}/libs/mexsupport)

include_directories(${SRC_MEXSUPPORT_DIR}/include )

set(SOURCES_MEXSUPPORT
	${SRC_MEXSUPPORT_DIR}/src/matlabAdapter.cpp
)

add_compile_definitions(COMPONENT_MEXSUPPORT)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_MEXSUPPORT}
)
# end lib mexsupport
