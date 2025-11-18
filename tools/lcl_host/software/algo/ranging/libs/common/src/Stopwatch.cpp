/***********************************************************************************/
/*!
 *  @brief      
 *  @file       Stopwatch.cpp
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#include <common/include/Stopwatch.h>

#if __ARM_ARCH && USE_STOPWATCH
/* time measurment tools */
stopwatch_time_struc stopwatch_tab[STOPWATCH_TIME_STRUC_SIZE] = {STOPWATCH_TRACE_UNDEF,0};
stopwatch_time_struc stopwatch_max = {STOPWATCH_TRACE_UNDEF,0};

int current_time_measurement = 0;
uint32_t nb_calculate_time_measurement = 0;
uint32_t sum_calculate_time_measurement = 0;
uint32_t max_calculate_time_measurement = 0;
uint32_t min_calculate_time_measurement = UINT32_MAX;

/* function that display every measurment of time taken, then rest time measurment index to zero */
void stopwatch_display(void) {
        for(int i=0;i < current_time_measurement;i++){
          switch(stopwatch_tab[i].id){
          case STOPWATCH_TRACE_EVD :
            printf("Evd time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_H2COV :
            printf("H2cov time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_H2COV_INPLACE :
            printf("H2cov_inplace time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_MAXEIG :
            printf("Maxeig time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_DORUN_UPDATE :
            printf("Dorun_update time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_FIRSTPEAK_PS :
            printf("FirstPeak_PS time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_CALCULATE :
            printf("Calculate time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_RECONSTRUCT2D :
            printf("ChannelReconstruct_2D time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_RECONSTRUCTNONPHASECOHERENT :
            printf("reconstructNonPhaseCoherent time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          case STOPWATCH_TRACE_EVD_SAES:
            printf("selfAdjointEigenSolver time measurment for measure %i in stopwatch_tab : \n",i);
            break;
          default :
            printf("Warning function not defined for time measurment\n");
          }
          if(stopwatch_tab[i].id == STOPWATCH_TRACE_CALCULATE){
            printf("%u microseconds\n\n",stopwatch_tab[i].stopwatch_time);
            nb_calculate_time_measurement++;
            max_calculate_time_measurement = MAX(max_calculate_time_measurement,stopwatch_tab[i].stopwatch_time);
            min_calculate_time_measurement = MIN(min_calculate_time_measurement,stopwatch_tab[i].stopwatch_time);
            sum_calculate_time_measurement = sum_calculate_time_measurement+stopwatch_tab[i].stopwatch_time;
          }else{
            printf("%u microseconds\n",stopwatch_tab[i].stopwatch_time);
          }
          stopwatch_tab[i].id = STOPWATCH_TRACE_UNDEF;
          stopwatch_tab[i].stopwatch_time = 0;
        }
        current_time_measurement = 0;
}

/* display summary of the stopwatch measurment */
void stopwatch_display_stat(void) {
    printf("Max time taken by a complete MUSIC test : %u microseconds\nMin time taken by a complete MUSIC test : %u microseconds\nAverage time taken by a complete MUSIC test : %u microseconds\n",max_calculate_time_measurement,min_calculate_time_measurement,(uint32_t)sum_calculate_time_measurement/nb_calculate_time_measurement);
    sum_calculate_time_measurement = 0;
    max_calculate_time_measurement = 0;
    min_calculate_time_measurement = UINT32_MAX;
    nb_calculate_time_measurement = 0; 
}
/* display the algorithm who took the most time to execute */
void stopwatch_display_max(void){
   switch(stopwatch_max.id){
          case STOPWATCH_TRACE_EVD :
            printf("Longest algorithm Evd took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_H2COV :
            printf("Longest algorithm H2cov took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_H2COV_INPLACE :
            printf("Longest algorithm H2cov_inplace took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_MAXEIG :
            printf("Longest algorithm Maxeig took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_DORUN_UPDATE :
            printf("Longest algorithm Dorun_update took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_FIRSTPEAK_PS :
            printf("Longest algorithm FirstPeak_PS took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_RECONSTRUCTNONPHASECOHERENT :
            printf("Longest algorithm reconstructNonPhaseCoherent took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_RECONSTRUCT2D :
            printf("Longest algorithm ChannelReconstruct_2D took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          case STOPWATCH_TRACE_EVD_SAES:
            printf("Longest algorithm selfAdjointEigenSolver took %u microseconds to finish\n",stopwatch_max.stopwatch_time);
            break;
          default :
            printf("Warning function not defined for time measurment\n");
          }
  stopwatch_max.id = STOPWATCH_TRACE_UNDEF;
  stopwatch_max.stopwatch_time = 0;
}
#endif

namespace ranging {

size_t Stopwatch::m_nesting = 0;

char  Stopwatch::m_ns[32] = { 0 };

}

// end
