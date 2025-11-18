#
# Append source files and flags for lib testsupport
#
set(SRC_TESTSUPPORT_DIR ${PROJECT_DIR}/libs/testsupport)

include_directories(${SRC_TESTSUPPORT_DIR}/include )

set(SOURCES_TESTSUPPORT
	${SRC_TESTSUPPORT_DIR}/src/test_utils.cpp
)

add_compile_definitions(COMPONENT_TESTSUPPORT)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_TESTSUPPORT}
)

# end lib testsupport
