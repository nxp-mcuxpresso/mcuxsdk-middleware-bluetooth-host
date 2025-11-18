#ifndef H_EST_DATA_H_
#define H_EST_DATA_H_

#include <music/include/MusicRanging.h>

/* number of position data in H_est_data.cpp */
extern const int kC;
/* number of antenna */
extern const int kA;
/* array pointing to all H_est from H_est_data.cpp */
extern const std::complex<float> *_H_est_array[];
/* array of all H_est true distance from H_est_data.cpp */
extern const float _H_est_true_distance[];

#endif /* H_EST_DATA_H_ */