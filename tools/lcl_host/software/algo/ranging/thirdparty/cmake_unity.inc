#
# Append source files and flags for Unity
#

include_directories(${PROJECT_DIR}/thirdparty/Unity/Unity/src )

# append to sources
set(SOURCES
	${SOURCES}
	${PROJECT_DIR}/thirdparty/Unity/Unity/src/unity.c
)

add_compile_definitions(UNITY_EXCLUDE_SETJMP_H=1)

# end Unity

