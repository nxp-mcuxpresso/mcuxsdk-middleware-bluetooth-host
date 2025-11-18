/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Stopwatch.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/
#ifndef UTILS_STOPWATCH_H_
#define UTILS_STOPWATCH_H_

#include <common/include/noncopyable.hpp>
#include <string.h>

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)


#if __ARM_ARCH
#if USE_STOPWATCH
#define STOPWATCH(s,r) ranging::Stopwatch TOKENPASTE2(_st_, __LINE__)(s,r)
#include "SystemTime.hpp"
#else
#define STOPWATCH(s,r)
#endif
#else
#if USE_STOPWATCH
#define STOPWATCH(s) ranging::Stopwatch TOKENPASTE2(_st_, __LINE__)(s)
#include "SystemTime.hpp"
#else
#define STOPWATCH(s)
#endif
#endif
#include <music/include/MusicRanging.h>
#include <stdio.h>


#ifndef STOPWATCH_MAXNESTING
#define STOPWATCH_MAXNESTING		255
#endif

#if  __ARM_ARCH && USE_STOPWATCH


#ifndef STOPWATCH_TIME_STRUC_SIZE
#define STOPWATCH_TIME_STRUC_SIZE 15
#endif

/* enum containing evrey algorithm used by MUSIC tests */
enum stopwatch_trace_id{
  STOPWATCH_TRACE_UNDEF,
  STOPWATCH_TRACE_RECONSTRUCTNONPHASECOHERENT,
  STOPWATCH_TRACE_RECONSTRUCT2D,
  STOPWATCH_TRACE_EVD,
  STOPWATCH_TRACE_EVD_SAES,
  STOPWATCH_TRACE_H2COV,
  STOPWATCH_TRACE_H2COV_INPLACE,
  STOPWATCH_TRACE_MAXEIG,
  STOPWATCH_TRACE_DORUN_UPDATE,
  STOPWATCH_TRACE_FIRSTPEAK_PS,
  STOPWATCH_TRACE_CALCULATE
};
/* struct for associating stopwatch_trace_id with a stopwatch_time measurment */
struct stopwatch_time_struc{
  stopwatch_trace_id id;
  uint32_t stopwatch_time;
};
extern stopwatch_time_struc stopwatch_max;
extern stopwatch_time_struc stopwatch_tab[STOPWATCH_TIME_STRUC_SIZE];
extern int current_time_measurement;

void stopwatch_display(void);
void stopwatch_display_stat(void);
void stopwatch_display_max(void);

namespace ranging {

class Stopwatch : private NonCopyable {

public:
	Stopwatch(const char *str1, stopwatch_trace_id id1) :
			m_str1(str1), m_id1(id1) {
		if (m_nesting <= STOPWATCH_MAXNESTING)
                  m_begin = get_timestamp();
                  m_nesting++;
	}

	~Stopwatch() {
		uint32_t ms_end = get_timestamp();
		m_nesting--;
		if (m_nesting <= STOPWATCH_MAXNESTING){
                  /* time measurment for the current algorithm m_id1 */
                  stopwatch_tab[current_time_measurement].id = m_id1;
                  if( m_begin > ms_end){
                    m_begin = COUNT_TO_USEC(UINT32_MAX,CLOCK_GetCoreSysClkFreq()) - m_begin;
                    stopwatch_tab[current_time_measurement].stopwatch_time = (uint32_t) (ms_end + m_begin);
                  }else{
                    stopwatch_tab[current_time_measurement].stopwatch_time = (uint32_t) (ms_end - m_begin);
                  }
                  /* check if a new max measurment has been found or not */
                  if( (stopwatch_tab[current_time_measurement].id != STOPWATCH_TRACE_CALCULATE) && (stopwatch_tab[current_time_measurement].stopwatch_time > stopwatch_max.stopwatch_time) ){
                    stopwatch_max.stopwatch_time = stopwatch_tab[current_time_measurement].stopwatch_time;
                    stopwatch_max.id = stopwatch_tab[current_time_measurement].id;
                  }
                  current_time_measurement++;

		}
	}

private:
	uint32_t get_timestamp() const {
		return SystemTime::get();
	}

	const char * indent() {
		if (m_nesting && ((m_nesting + 1) < sizeof(m_ns))) {
			memset(m_ns, '\t', m_nesting);
			m_ns[m_nesting] = '\0';
			return m_ns;
		} else {
			m_ns[0] = '\0';
			return m_ns;
		}
	}

private:
	Stopwatch() = delete; /* hide */

private:
	const char * m_str1;
	uint32_t m_begin;
	static size_t m_nesting;
	static char m_ns[32];
        stopwatch_trace_id m_id1;
};

} /* namespace ranging */

#else

namespace ranging {

class Stopwatch : private NonCopyable {

public:
	Stopwatch(const char *str1) :
			m_str1(str1) {
                          
		if (m_nesting <= STOPWATCH_MAXNESTING)
			printf("---> %s Stopwatch: %s\n", indent(), m_str1);
		m_begin = get_timestamp();
		m_nesting++;
	}

	~Stopwatch() {
		long int ms_end = get_timestamp();
		m_nesting--;
		if (m_nesting <= STOPWATCH_MAXNESTING){
                  printf("<--- %s Stopwatch: %s : elapsed %u us\n", indent(), m_str1, (unsigned int) (ms_end - m_begin));
		}
	}

private:
	int get_timestamp() const {
#if USE_STOPWATCH
		return SystemTime::get();
#else
		return 0;
#endif
	}

	const char * indent() {
		if (m_nesting && ((m_nesting + 1) < sizeof(m_ns))) {
			memset(m_ns, '\t', m_nesting);
			m_ns[m_nesting] = '\0';
			return m_ns;
		} else {
			m_ns[0] = '\0';
			return m_ns;
		}
	}

private:
	Stopwatch() = delete; /* hide */

private:
	const char * m_str1;
	long int m_begin;
	static size_t m_nesting;
	static char m_ns[32];
};

} /* namespace ranging */
#endif

#endif /* UTILS_STOPWATCH_H_ */
