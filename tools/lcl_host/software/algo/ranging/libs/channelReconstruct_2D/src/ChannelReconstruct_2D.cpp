
#include <common/include/Functions.h>
#include <common/include/Stopwatch.h>
#include <common/include/Stream.h>
#include <common/include/VectorTypes.h>
#include <common/include/Constants.h>
#include <common/include/memory/memory.hpp>
#include <channelReconstruct_2D/include/ChannelReconstruct_2D.h>
#include <crNonPhaseCoherent/include/reconstructNonPhaseCoherent.h>

#if USE_MEMTRACE
#include <common/include/memtrace.h>
#endif

#include <assert.h>

#define REC2D_SHOW_INPUTS               0       /* 1 = on, 0 = off */
#define REC2D_SHOW_OUTPUTS              0       /* 1 = on, 0 = off */

namespace ranging {

//original implementation where IQ had saturated path removed beforehand
void ChannelReconstruct_2D(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert) {
    TRACE();

#if  __ARM_ARCH
    STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_RECONSTRUCT2D);
#else
    STOPWATCH(__FUNCTION__);
#endif

#if REC2D_SHOW_INPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  in IQ_A   =   " << IQ_A;
    std::cout << __FUNCTION__ << "  in IQ_B   =   " << IQ_B;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

#if RANGE_CHECK
    if ((IQ_A.rows() != IQ_B.rows()) || (IQ_A.cols() != IQ_B.cols()))
        throw std::range_error("ChannelReconstruct_2D input matrices do not match");
    if ((H_est.rows() != IQ_A.rows()) || (H_est.cols() != IQ_A.cols()))
        throw std::range_error("ChannelReconstruct_2D input matrices do not match");
    if ((IQ_A.rows() != K_est.cols()))
        throw std::range_error("ChannelReconstruct_2D input Kest wrong size");
    if ((IQ_A.rows() != Uncert.cols()))
        throw std::range_error("ChannelReconstruct_2D input Uncert wrong size");
#endif

    MatrixCmpl IQ_ref(IQ_A.rows(), IQ_A.cols(), IQ_A.data());

    /* call reconstructNonPhaseCoherent */
    reconstructNonPhaseCoherent(IQ_ref, IQ_B, H_est, K_est, Uncert);
    
    if(IQ_ref.rows() != 1)
    {
        msize_t Ka = IQ_ref.rows();
        msize_t Kf = IQ_ref.cols();
        MatrixF d_high(Ka-1, 1, MATRIX_UNINIT_DATA);
        MatrixF d_low(Ka-1, 1, MATRIX_UNINIT_DATA);
        float d_high_;
        float d_low_;

        MatrixCmpl diff_IQ_ref(Ka-1, Kf, MATRIX_UNINIT_DATA);
        ComplexF diff_H_est = ComplexF(0,0);
        
        IQ_ref = IQ_ref * expCmpl(angle(H_est(0, 0)) - angle(IQ_ref(0, 0)));

        for (msize_t rowcount = 0; rowcount < Ka-1; rowcount++)
        {
            for (msize_t col = 0; col < Kf; col++)
            {
                diff_IQ_ref(rowcount, col) = IQ_ref(rowcount, col) * conj(IQ_ref(rowcount+1, col)) / abs(IQ_ref(rowcount+1, col));
            }
        }
        for(msize_t itone = 0; itone < Kf; itone++)
        {
            /* Overall antenna sign - flip correction */
            if(itone == 0)
            {
                for (msize_t rowcount = 0; rowcount < Ka; rowcount++)
                {
                    d_high_ = abs(H_est(rowcount, itone) - IQ_ref(rowcount, itone));
                    d_low_ = abs(-H_est(rowcount, itone) - IQ_ref(rowcount, itone));
                    if (d_high_ > d_low_)
                    {
                        for (msize_t itone2 = itone; itone2 < H_est.cols(); itone2++)
                        {
                            H_est(rowcount, itone2) = -H_est(rowcount, itone2);
                        }
                    }
                }
                continue;
            }
            for(msize_t rowcount = 0; rowcount < Ka-1; rowcount++)
            {
                diff_H_est = H_est(rowcount, itone) * conj(H_est(rowcount+1, itone)) / (norm(H_est(rowcount+1, itone)) + (float) std::pow(10, -10) );
                d_high(rowcount,0) = abs(diff_H_est - diff_IQ_ref(rowcount, itone));
                d_low(rowcount,0) = abs(-diff_H_est - diff_IQ_ref(rowcount, itone));
            }
            /* antenna sign - flip correction tone - by - tone */
            for(msize_t iant = 0; iant < (Ka-1); iant++)
            {
 
                if(d_high(iant, 0) > d_low(iant, 0))
                {
                    msize_t iant2blame = 0;
                    /* something wrong with the current antenna pair
                    try to tell which antenna's fault */
                    if (Ka == 2)
                    {
                        /* if only 2 antenna available, the one with higher magnitude is trusted */
                        float min_value = abs(H_est(0,itone));
                        for (msize_t rowcount = 1; rowcount < H_est.rows(); rowcount++) 
                        {
                            if( abs(H_est(rowcount, itone)) < min_value)
                            {
                                iant2blame = rowcount;
                                min_value = abs(H_est(rowcount, itone));
                            }
                        }
                    }
                    else if (iant == (Ka - 2))
                    {
                        /* if we've come to the last antenna pair, it's definitely
                        the latter antenna's fault */
                        iant2blame = iant + 1;
                    }
                    else
                    {
                        if (d_high(iant+1, 0) > d_low(iant+1, 0))
                        {
                            /* something also wrong with the next antenna pair */
                            iant2blame = iant + 1;
                        }
                        else
                        {
                            iant2blame = iant;
                        }
                    }
                    for(msize_t colum = itone; colum < H_est.cols(); colum++)
                    {
                        H_est(iant2blame, colum) = -H_est(iant2blame, colum);
                    }
                }
            }
        }
    }
#if USE_MEMTRACE
        MEMTRACE_MARKER("ChannelReconstruct_2D");
#endif

#if REC2D_SHOW_OUTPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  out H_est   =   " << H_est;
    std::cout << __FUNCTION__ << "  out K_est   =   " << K_est;
    std::cout << __FUNCTION__ << "  out Uncertainty  =   " << Uncert;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

}
/* saturated_array indicates which antenna paths are saturated and should be ignored for H_est computation*/
void ChannelReconstruct_2D(const MatrixCmpl & IQ_A, const MatrixCmpl & IQ_B, MatrixCmpl & H_est, MatrixF & K_est, Matrix<int> & Uncert, const int *saturated_array) 
{
    TRACE();

#if  __ARM_ARCH
    STOPWATCH(__FUNCTION__,STOPWATCH_TRACE_RECONSTRUCT2D);
#else
    STOPWATCH(__FUNCTION__);
#endif

#if REC2D_SHOW_INPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  in IQ_A   =   " << IQ_A;
    std::cout << __FUNCTION__ << "  in IQ_B   =   " << IQ_B;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

#if RANGE_CHECK
    if ((IQ_A.rows() != IQ_B.rows()) || (IQ_A.cols() != IQ_B.cols()))
        throw std::range_error("ChannelReconstruct_2D input matrices do not match");
    if ( (H_est.cols() != IQ_A.cols()))
        throw std::range_error("ChannelReconstruct_2D input matrices do not match");
    if ((H_est.rows() != K_est.cols()) || (K_est.rows() != 1))
        throw std::range_error("ChannelReconstruct_2D input Kest wrong size");
    if ((H_est.rows() != Uncert.cols()) || (Uncert.rows() != 1))
        throw std::range_error("ChannelReconstruct_2D input Uncert wrong size");
#endif    

 
    if(H_est.rows() != 1)
    {     
        
        /* reconstructNonPhaseCoherent */
        reconstructNonPhaseCoherent(IQ_A, IQ_B, H_est, K_est, Uncert, saturated_array);
     
        MatrixCmpl IQ_ref(H_est.rows(), H_est.cols(),MATRIX_UNINIT_DATA );
        msize_t row_h_est = 0 ;
        for (msize_t i = 0; i < IQ_A.rows(); i++){
            if(saturated_array[i] == 0){
                IQ_ref.update(Span(row_h_est,row_h_est),Span(0,IQ_A.cols()-1), IQ_A.getRow(i));
                row_h_est++;
            }
        }

        msize_t Ka = IQ_ref.rows();
        msize_t Kf = IQ_ref.cols();
        //MatrixF d_high(Ka-1, 1, MATRIX_UNINIT_DATA);
        //MatrixF d_low(Ka-1, 1, MATRIX_UNINIT_DATA);
        //float d_high_;
        //float d_low_;
        //Marix float might not be necessary since only 1 Dimension arrays
        float d_high[4-1];
        float d_low[4-1];

        MatrixCmpl diff_IQ_ref(Ka-1, Kf, MATRIX_UNINIT_DATA);
        ComplexF diff_H_est = ComplexF(0,0);
        
        IQ_ref = IQ_ref * expCmpl(angle(H_est(0, 0)) - angle(IQ_ref(0, 0)));

        for (msize_t rowcount = 0; rowcount < Ka-1; rowcount++)
        {
            for (msize_t col = 0; col < Kf; col++)
            {
                diff_IQ_ref(rowcount, col) = IQ_ref(rowcount, col) * conj(IQ_ref(rowcount+1, col)) / abs(IQ_ref(rowcount+1, col));
            }
        }
        for(msize_t itone = 0; itone < Kf; itone++)
        {
            /* Overall antenna sign - flip correction */
            if(itone == 0)
            {
                for (msize_t rowcount = 0; rowcount < Ka; rowcount++)
                {
                    d_high[0] = abs(H_est(rowcount, itone) - IQ_ref(rowcount, itone));
                    d_low[0] = abs(-H_est(rowcount, itone) - IQ_ref(rowcount, itone));
                    if (d_high[0] > d_low[0])
                    {
                        for (msize_t itone2 = itone; itone2 < H_est.cols(); itone2++)
                        {
                            H_est(rowcount, itone2) = -H_est(rowcount, itone2);
                        }
                    }
                }
                continue;
            }
            for(msize_t rowcount = 0; rowcount < (Ka-1); rowcount++)
            {
                diff_H_est = H_est(rowcount, itone) * conj(H_est(rowcount+1, itone)) / (abs(H_est(rowcount+1, itone)) + (float) std::pow(10, -10) );
                d_high[rowcount] = abs(diff_H_est - diff_IQ_ref(rowcount, itone));
                d_low[rowcount] = abs(-diff_H_est - diff_IQ_ref(rowcount, itone));
            }
            /* antenna sign - flip correction tone - by - tone */
            for(msize_t iant = 0; iant < (Ka-1); iant++)
            {
                if(d_high[iant] > d_low[iant])
                {
                    msize_t iant2blame = 0;
                    /* something wrong with the current antenna pair
                    try to tell which antenna's fault */
                    if (Ka == 2)
                    {
                        /* if only 2 antenna available, the one with higher magnitude is trusted */
                        float min_value = abs(H_est(0,itone));
                        for (msize_t rowcount = 1; rowcount < H_est.rows(); rowcount++) 
                        {
                            if( abs(H_est(rowcount, itone)) < min_value)
                            {
                                iant2blame = rowcount;
                                min_value = abs(H_est(rowcount, itone));
                            }
                        }
                    }
                    else if (iant == (Ka - 2))
                    {
                        /* if we've come to the last antenna pair, it's definitely
                        the latter antenna's fault */
                        iant2blame = iant + 1;
                    }
                    else
                    {
                        if (d_high[iant+1] > d_low[iant+1])
                        {
                            /* something also wrong with the next antenna pair */
                            iant2blame = iant + 1;
                        }
                        else
                        {
                            iant2blame = iant;
                        }
                    }
                    for(msize_t colum = itone; colum < H_est.cols(); colum++)
                    {
                        H_est(iant2blame, colum) = -H_est(iant2blame, colum);
                    }
                }
            }
        }
    }
    else
    {
        /* reconstructNonPhaseCoherent */
        reconstructNonPhaseCoherent(IQ_A, IQ_B, H_est, K_est, Uncert);
    }
#if USE_MEMTRACE
        MEMTRACE_MARKER("ChannelReconstruct_2D");
#endif

#if REC2D_SHOW_OUTPUTS
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
    std::cout << __FUNCTION__ << "  out H_est   =   " << H_est;
    std::cout << __FUNCTION__ << "  out K_est   =   " << K_est;
    std::cout << __FUNCTION__ << "  out Uncertainty  =   " << Uncert;
    std::cout << __FUNCTION__ << "  --------------- " << std::endl;
#endif

}

} /* namespace ranging */
