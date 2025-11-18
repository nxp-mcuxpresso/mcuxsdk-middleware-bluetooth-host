/***********************************************************************************/
/*!
 *  @brief      
 *  @file       PythonWrapper.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef PYTHONWRAPPER_H_
#define PYTHONWRAPPER_H_


#include <vector>
#include <complex>

#include <common/include/Matrix.h>
#include <common/include/Types.h>
#include <common/include/RangingSettings.h>

typedef int msize_t;

class CmplxMatrixType {
	CmplxMatrixType() = delete;

public:
	CmplxMatrixType(int _rows, int _cols) : m_rows(_rows), m_cols(_cols) {
		data.resize(_rows * _cols);
	}

	inline size_t size() {
		return data.size();
	}

public:
	inline msize_t rows() const { return m_rows; }
	inline msize_t cols() const { return m_cols; }

	/* change size of Matrix, memory not cleared */
	void resize(int _rows, int _cols) {
		m_rows = _rows;
		m_cols = _cols;
		data.resize(_rows * _cols);
	}
public:
	std::vector< std::complex<float> > data;

private:
	int m_rows;
	int m_cols;
};

class DataPhMag {
	DataPhMag() = delete;

public:
	DataPhMag(int _rows, int _cols) : H_est(_rows,_cols), m_rows(_rows), m_cols(_cols) {
		int asize = _rows * _cols;
		phaseA.resize(asize);
		phaseB.resize(asize);
		magnitudeA.resize(asize);
		magnitudeB.resize(asize);
	}

	size_t size() {
		return phaseA.size();
	}

	std::vector<float> phaseA;
	std::vector<float> phaseB;
	std::vector<float> magnitudeA;
	std::vector<float> magnitudeB;

	CmplxMatrixType H_est;

public:
	msize_t rows() const { return m_rows; }
	msize_t cols() const { return m_cols; }

private:
	int m_rows;
	int m_cols;
};

class DataIQ {
	DataIQ() = delete;

public:
	DataIQ(int _rows, int _cols) : H_est(_rows,_cols), m_rows(_rows), m_cols(_cols) {
		int asize = _rows * _cols;
		IQ_A.resize(asize);
		IQ_B.resize(asize);
		Kest.resize(_rows);
		Uncertainty.resize(_rows);
	}

	size_t size() {
		return IQ_A.size();
	}

	std::vector< std::complex<float> > IQ_A;
	std::vector< std::complex<float> > IQ_B;

	CmplxMatrixType H_est;

	std::vector< float > Kest;
	std::vector< int > Uncertainty;

public:
	msize_t rows() const { return m_rows; }
	msize_t cols() const { return m_cols; }

private:
	int m_rows;
	int m_cols;
};

class MusicData {
	MusicData() = delete;

public:
	MusicData(int _rows, int _cols, const float _delta_F, const int _L) :
			hest(_rows,_cols), EigenVects(_L,_L), delta_F(_delta_F), L(_L), m_rows(_rows), m_cols(_cols) {
		EigenValues.resize(L);
	}

	msize_t rows() const { return m_rows; }
	msize_t cols() const { return m_cols; }

	/* Reset measurement instance to prepare for new distance calculation */
	void reset() {
		EigenVects.resize(L, L);
		EigenValues.resize(L);
	}

public:
	CmplxMatrixType hest;
	CmplxMatrixType EigenVects;
	std::vector<float> EigenValues;

	const float delta_F;
	const int L;

private:
	int m_rows;
	int m_cols;
};

/* Make sure these are aligned with defines in MusicRanging.h */
#define ALGO_TYPE_EVD			0	/* IMEC EVD Power Iterations */
#define ALGO_TYPE_EIGEN_SAES	1	/* Self Adjoint Eigen Solver */


class Worker {
public:

	void crNonPhaseCoherent(DataIQ &m) const;
	float calculateCrNonPhaseCoherent(DataIQ &m, const float delta_F, const int L);

	void crPhaseCoherent(DataPhMag &m) const;
	float calculateCrPhaseCoherent(DataPhMag &m, const float delta_F, const int L);

	float calculateDistance(MusicData &m, int evdType = ALGO_TYPE_EVD); /* default algo selection */

	void showSettings() const;

	ranging::RangingSettings settings;
};

#endif /* PYTHONWRAPPER_H_ */
