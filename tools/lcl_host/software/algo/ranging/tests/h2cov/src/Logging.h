/***********************************************************************************/
/*!
 *  @brief      
 *  @file       RLogging.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef LOGGING_H_
#define LOGGING_H_

#ifndef RANGING_VERBOSE
#define RANGING_VERBOSE      0   /* 0 = off, 1 = on */
#endif

#if RANGING_VERBOSE
#include <stdio.h>
#define MTRACE(rows,cols)    do { printf("++ %s (%d,%d)\n", __PRETTY_FUNCTION__ , rows , cols); } while (0)
#define TRACE(x)             do { printf("++ %s\n", __PRETTY_FUNCTION__ ); } while (0)
#else
#define MTRACE(rows,cols)    do { /* empty */ } while (0)
#define TRACE(x)             do { /* empty */ } while (0)
#endif

//#define DPRINTF(fmt, ...)  do { printf("%s:%d  " fmt , __FILE__, __LINE__ , ## __VA_ARGS__); } while (0)
//#define DPRINTF(fmt, ...)  do { printf(fmt, ## __VA_ARGS__); } while (0)
#define DPRINTF(fmt, ...)    do { /* empty */ } while (0)


#endif /* LOGGING_H_ */
