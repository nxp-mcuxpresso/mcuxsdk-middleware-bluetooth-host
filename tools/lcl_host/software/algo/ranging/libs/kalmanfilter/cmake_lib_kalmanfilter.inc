#
# Append source files and flags for lib kalmanfilter
#
set(SRC_FKALMAN_DIR ${PROJECT_DIR}/libs/kalmanfilter)

include_directories(${SRC_FKALMAN_DIR}/include )

set(SOURCES_FKALMAN
	${SRC_FKALMAN_DIR}/src/AdaptiveKalmanFilter.cpp
	${SRC_FKALMAN_DIR}/src/KalmanFilter.cpp
)

add_compile_definitions(COMPONENT_FKALMAN)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_FKALMAN}
)
# end lib kalmanfilter
