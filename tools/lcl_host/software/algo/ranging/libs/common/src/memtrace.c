/***********************************************************************************/
/*!
 *  @brief      
 *  @file       memtrace.c
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#if USE_MEMTRACE
/*
 * Link-time interception of malloc and free using the static
 * linker's (ld) "--wrap symbol" flag.
 *
 * Compile the executable using "-Wl,--wrap,malloc -Wl,--wrap,free".
 * This tells the linker to resolve references to malloc as
 * __wrap_malloc, free as __wrap_free, __real_malloc as malloc, and
 * __real_free as free.
 * Works with gcc compiler.
 *
*/
#include <stdio.h>
#include <assert.h>
#if __ARM_ARCH
#include "fsl_common.h"
#endif

#define MEMTRACE_MAP_SIZE		64

struct alloc_map_s {
	void *ptr;
	size_t size;
};

struct alloc_map_s amap[MEMTRACE_MAP_SIZE] = { NULL };

/* Allow to display malloc details when set to 1 */
static int DISPLAY_MALLOC_DETAILS_ALGO = 1;
static int DISPLAY_MALLOC_DETAILS = 0;

static int mem_allocated = 0;
static int mem_allocated_peak = 0;
static int mem_allocated_peak_max = 0;
static int mem_allocated_peak_summary = 0;

#if defined(__ICCARM__)
extern void* $Super$$__iar_dlmalloc(size_t);
extern void $Super$$__iar_dlfree(void *);
#else
void* __real_malloc(size_t size);
void __real_free(void *ptr);
#endif
/*
 * __wrap_malloc - malloc wrapper function
 */
#if defined(__ICCARM__)
void* $Sub$$__iar_dlmalloc(size_t size) {
        void *ptr =$Super$$__iar_dlmalloc(size);
#else
void* __wrap_malloc(size_t size) {
        void *ptr =__real_malloc(size);
#endif
	for (int n = 0; n < MEMTRACE_MAP_SIZE; n++) {
		if (amap[n].ptr == NULL) {
			amap[n].ptr = ptr;
			amap[n].size = size;
			mem_allocated += size;
			mem_allocated_peak = (mem_allocated_peak>mem_allocated)?mem_allocated_peak:mem_allocated;
                        if( DISPLAY_MALLOC_DETAILS == 1 ){
                          printf("MEMTRACE malloc,%d,%d\n", (int)size, mem_allocated);
                        }
			return ptr;
		}
	}
	assert(0 && "increase MEMTRACE_MAP_SIZE");
	printf("** failed to store malloc entry\n");
	return ptr;
}

/*
 * __wrap_free - free wrapper function
 */
#if defined(__ICCARM__)
void $Sub$$__iar_dlfree(void *ptr) {
#else
void __wrap_free(void *ptr) {
#endif
	for (int n = 0; n < MEMTRACE_MAP_SIZE; n++) {
		if (amap[n].ptr == ptr) {
			amap[n].ptr = NULL;
			size_t size = amap[n].size;
			mem_allocated -= size;
                        if( DISPLAY_MALLOC_DETAILS == 1 ) {
                          printf("MEMTRACE free,%d,%d\n", (int)size, mem_allocated);
                        }
#if defined(__ICCARM__)
                        $Super$$__iar_dlfree(ptr);
#else
                        __real_free(ptr);
#endif
                        return;
		}
	}
	assert(0 && "increase MEMTRACE_MAP_SIZE");
	printf("** failed to free malloc entry\n");
#if defined(__ICCARM__)
        $Super$$__iar_dlfree(ptr);
#else
        __real_free(ptr);
#endif
}
/* display current malloc peak and reset the memtrace to 0 */
void memtrace_marker(const char *tname) {
        mem_allocated_peak_max = MAX(mem_allocated_peak_max,mem_allocated_peak);
        if(DISPLAY_MALLOC_DETAILS_ALGO  == 1 ){
          printf("MEMTRACE peak,%s, %d\n", tname, mem_allocated_peak);
        }
	mem_allocated_peak = 0;
}
/* display current malloc max peak and reset the max memtrace to 0, store the result in the meax peak summary tracer */
void memtrace_marker_peak(const char *tname) {
        mem_allocated_peak_max = MAX(mem_allocated_peak_max,mem_allocated_peak);
        mem_allocated_peak_summary = MAX(mem_allocated_peak_max,mem_allocated_peak_summary);
        if(DISPLAY_MALLOC_DETAILS_ALGO  == 1 ){
          printf("MEMTRACE max peak,%s, %d\n", tname, mem_allocated_peak_max);
        }
        mem_allocated_peak = 0;
        mem_allocated_peak_max = 0;
}
/* display current malloc max peak summary and don't reset the summary memtrace to 0 */
void memtrace_marker_peak_summary(const char *tname) {
        mem_allocated_peak_summary = MAX(mem_allocated_peak_max,mem_allocated_peak_summary);
        /* average is calaulated according to all memory peak measured by memtracer marker and number of peak recorded by memtracer marker peak*/
        printf("MEMTRACE summary max peak,%s, %d\n", tname, mem_allocated_peak_summary);
        mem_allocated_peak_summary = 0;
}

void memtrace_marker2(const char *tname, const char *tpar) {
        if(DISPLAY_MALLOC_DETAILS_ALGO  == 1 ){
          char name[256];
          sprintf(name,"%s_%s", tname, tpar);
          printf("MEMTRACE peak,%s, %d\n", name, mem_allocated_peak);
        }
	mem_allocated_peak = 0;
}
#endif
// end

