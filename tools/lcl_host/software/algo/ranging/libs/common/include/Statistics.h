/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Statistics.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef UTILS_STATISTICS_H_
#define UTILS_STATISTICS_H_

#if USE_STATISTICS

namespace ranging {

struct stats_s {
	struct {
		int alloc;
		int free;
	} n;
	struct {
		int alloc;
		int free;
	} c;
};

extern struct stats_s stats;


void printval(const char *s, int v);

} /* namespace ranging */


#define STATS_INCR(x)	do { stats.n.x = stats.n.x + 1; } while (0)

#define STATS_COUNT(x,v) do { stats.c.x = stats.c.x + v; STATS_INCR(x); printval(#x, stats.c.x);} while (0);


#else

#define STATS_COUNT(x,v) do { /* empty */} while (0);

#endif

#endif /* UTILS_STATISTICS_H_ */
