/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Statistics.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/


#if USE_STATISTICS

#include <common/include/Statistics.h>
#include <stdio.h>

namespace ranging {

struct stats_s stats;

void printval(const char *s, int v) {

	//printf("STATS %d %d %d %d\n",stats.n.alloc, stats.n.free, stats.c.alloc, stats.c.free);
	printf("STATS %d,%d,%d,%d,%d\n", stats.n.alloc, stats.n.free, stats.c.alloc, stats.c.free, stats.c.alloc - stats.c.free);
}

} /* namespace ranging */

#endif

// end
