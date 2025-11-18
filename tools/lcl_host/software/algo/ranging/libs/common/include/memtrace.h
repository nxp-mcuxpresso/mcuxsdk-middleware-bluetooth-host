/***********************************************************************************/
/*!
 *  @brief      
 *  @file       memtrace.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef MEMTRACE_H
#define MEMTRACE_H

#if USE_MEMTRACE
#define MEMTRACE_MARKER2(a, b) memtrace_marker2(a, b)
#define MEMTRACE_MARKER(a) memtrace_marker(a)
#define MEMTRACE_MARKER_PEAK(a) memtrace_marker_peak(a)
#define MEMTRACE_MARKER_PEAK_SUMMARY(a) memtrace_marker_peak_summary(a)
#else
#define MEMTRACE_MARKER2(a, b) do { /* empty */ } while (0)
#define MEMTRACE_MARKER(a) do { /* empty */ } while (0)
#define MEMTRACE_MARKER_MAX(a) do { /* empty */ } while (0)
#define MEMTRACE_MARKER_PEAK_SUMMARY(a) do { /* empty */ } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void memtrace_marker(const char *tname);
void memtrace_marker2(const char *tname, const char *tpar);
void memtrace_marker_peak(const char *tname);
void memtrace_marker_peak_summary(const char *tname);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

