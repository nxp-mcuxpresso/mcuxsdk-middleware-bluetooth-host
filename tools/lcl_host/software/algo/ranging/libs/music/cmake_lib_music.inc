#
# Append source files and flags for lib music
#
set(SRC_MUSIC_DIR ${PROJECT_DIR}/libs/music)

include_directories(${SRC_MUSIC_DIR}/include )

set(SOURCES_MUSIC
	${SRC_MUSIC_DIR}/src/MusicRanging.cpp
)

add_compile_definitions(COMPONENT_MUSIC)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_MUSIC}
)
# end lib music
