#
# Append source files and flags for lib crNonPhaseCoherent
#
set(SRC_CRNPC_DIR ${PROJECT_DIR}/libs/crNonPhaseCoherent)

include_directories(${SRC_CRNPC_DIR}/include )

set(SOURCES_CRNPC
	${SRC_CRNPC_DIR}/src/reconstructNonPhaseCoherent.cpp
)

add_compile_definitions(COMPONENT_CRNPC)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_CRNPC}
)
# end lib crNonPhaseCoherent
