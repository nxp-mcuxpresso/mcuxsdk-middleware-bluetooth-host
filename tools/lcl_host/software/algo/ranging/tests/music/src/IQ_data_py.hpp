#ifndef IQ_DATA_PY_H_
#define IQ_DATA_PY_H_

#include <music/include/MusicRanging.h>

 /* number of position data in IQ_data_py.cpp */
extern const int kC_IQ;
/* number of antenna path */
extern const int kA_IQ;
/* number of tones */
extern const int kB_IQ;
/* array pointing to all IQ from IQ_data_py.cpp */
extern const ranging::ComplexF *_IQ_array[];
/* array pointing to all H_est from IQ_data_py.cpp */
extern const ranging::ComplexF *_H_est_array_div[];
/* array of all IQ true distance from IQ_data_py.cpp */
extern const float _IQ_array_true_distance[];

#endif /* IQ_DATA_PY_H_ */ 