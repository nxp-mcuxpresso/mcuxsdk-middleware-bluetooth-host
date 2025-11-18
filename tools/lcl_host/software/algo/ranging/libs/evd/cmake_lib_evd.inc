#
# Append source files and flags for lib evd
#
set(SRC_EVD_DIR ${PROJECT_DIR}/libs/evd)

include_directories(${SRC_EVD_DIR}/include )

set(SOURCES_EVD
	${SRC_EVD_DIR}/src/evd.cpp
)

add_compile_definitions(COMPONENT_EVD)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_EVD}
)
# end lib evd
