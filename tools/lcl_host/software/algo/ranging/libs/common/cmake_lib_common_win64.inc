#
# Append source files and flags for lib common
#
set(SRC_COMMON_DIR ${PROJECT_DIR}/libs/common)
set(ARM_COMMON_SRC ${SRC_COMMON_DIR}/src/arm/msvc)

include_directories(${SRC_COMMON_DIR}/include )
include_directories(${ARM_COMMON_SRC})

set(SOURCES_COMMON
	${ARM_COMMON_SRC}/arm_mat_add_f32.c
	${ARM_COMMON_SRC}/arm_mat_cmplx_mult_f32.c
	${ARM_COMMON_SRC}/arm_mat_init_f32.c
	${ARM_COMMON_SRC}/arm_mat_inverse_f32.c
	${ARM_COMMON_SRC}/arm_mat_mult_f32.c
	${ARM_COMMON_SRC}/arm_mat_scale_f32.c
	${ARM_COMMON_SRC}/arm_mat_sub_f32.c
	${ARM_COMMON_SRC}/arm_mat_trans_f32.c
	${SRC_COMMON_DIR}/src/Functions.cpp
	${SRC_COMMON_DIR}/src/memory/allocator.cpp
	${SRC_COMMON_DIR}/src/memory/memory.cpp
	${SRC_COMMON_DIR}/src/RangingSettings.cpp
)

add_compile_definitions(COMPONENT_COMMON)

# append to sources
set(SOURCES
	${SOURCES}
	${SOURCES_COMMON}
)
# end lib common
