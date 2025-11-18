/***********************************************************************************/
/*!
 *  @brief      
 *  @file       matlabAdapter.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef MATLABADAPTER_H_
#define MATLABADAPTER_H_


#include <common/include/Matrix.h>

#include "mex.h"

using namespace ranging;

mxArray* cpp2mat_complex(const MatrixCmpl & M);
mxArray* cpp2mat_float(const MatrixF & M);
mxArray* cpp2mat_integer(const Matrix<int> & M);

MatrixCmpl mat2cpp_complex(const mxArray *marray);
MatrixF mat2cpp_float(const mxArray *marray);

// end



#endif /* MATLABADAPTER_H_ */
