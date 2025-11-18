#
# Append source files and flags for lib channelReconstruct_2D
#
set(SRC_CR2D_DIR ${PROJECT_DIR}/libs/channelReconstruct_2D)

include_directories(${SRC_CR2D_DIR}/include )

set(SOURCES_CR2D
	${SRC_CR2D_DIR}/src/ChannelReconstruct_2D.cpp
)

add_compile_definitions(COMPONENT_CR2D)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_CR2D}
)
# end lib channelReconstruct_2D
