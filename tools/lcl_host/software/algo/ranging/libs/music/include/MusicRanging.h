/***********************************************************************************/
/*!
 *  @brief
 *  @file       MusicRanging.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note
 */
/***********************************************************************************/

#ifndef MUSIC_RANGING_H_
#define MUSIC_RANGING_H_

#include <common/include/Matrix.h>
#include <common/include/RangingSettings.h>

#define ALGO_TYPE_EVD			0	/* IMEC EVD Power Iterations */
#define ALGO_TYPE_EIGEN_SAES	1	/* Self Adjoint Eigen Solver */

/* if you want to define the EVD type used at run time, define this macro and use global_evd_type with the according values to run specific EVD type */
#if GLOBAL_DEF_EVD_TYPE
extern int global_evd_type;
#endif


namespace ranging {

class MusicRanging {

public:
    MusicRanging();
    MusicRanging(RangingSettings & r);

public:
    void calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, int evdType = ALGO_TYPE_EVD);
    void calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, float &likeliness, int evdType = ALGO_TYPE_EVD);
    void calculate(const float delta_F, const int L, const MatrixCmpl &H_est, float &dist_est, float &likeliness, MatrixCmpl &eigenvect, MatrixF &EVs, int evdType = ALGO_TYPE_EVD);
    void calculate_SRDE(const MatrixCmpl &_IQ_A, const MatrixCmpl &_IQ_B, unsigned int *FreqMask, const float delta_F, const int n_ap, const int nb_freq, float &dist_est, float &likeliness, int evdType = ALGO_TYPE_EVD);
    void calculate_Likeliness(const int L, msize_t Ns, float omega, float &CovTrace, MatrixCmpl &eigenvects, MatrixF &EVs, float &likeliness);
    void FirstPeak_PS(const MatrixCmpl &SigVects, float &omega, float &fval, const float delta_F, const msize_t L);
    msize_t SubSpaceSep(const MatrixF &EVs);

    RangingSettings & config() {
          return m_config;
    }

private:
    /* internal functions */
    MatrixCmpl fftvec(const msize_t N_b, const float omega);
    float InvPS(const MatrixCmpl &SigVects, const float omega, const msize_t L);
    msize_t check_saturated_paths(const MatrixCmpl &IQ_A, const MatrixCmpl &IQ_B, int *saturated_array);

private:
    RangingSettings m_config;
};

} /* namespace ranging */

#endif /* MUSIC_RANGING_H_ */
