#ifndef __SVM_SOLVER_H__
#define __SVM_SOLVER_H__

#include "KernelMatrix.h"

template<typename Comparable>
inline Comparable max2( Comparable a, Comparable b )
{
	return a > b ? a : b;
}

template<typename Comparable>
inline Comparable min2( Comparable a, Comparable b )
{
	return a < b ? a : b;
}

template<typename Comparable>
inline void min_rep( const Comparable &cur, Comparable &target, const int &cur_i, int &target_i )
{
	if ( cur < target )
	{
		target = cur;
		target_i = cur_i;
	}
}

#define BOUND_EPS 1e-12

/**
 * This is the alogrithm in libsvm 1.0
 * related paper: LIBSVM: Introduction and Benchmarks
 * The algorithm is very small and graceful, and it is 
 * a combination of SMO and SVM-Light.
 * 
 * It solves the fllowing problem
 * f(x) = 0.5 * alpha^T * Q alpha - e^T * alpha
 * s.t. 0 <= alpha[i] <= C, i = 1,...,M
 *      sum_i=1 to M (alpha[i]*y[i]) = 0
 */

class SVM_Solver
{
public:
	SVM_Solver( const SVM_Input *input );
	~SVM_Solver();
	void solve( SVM_Model &model );
private:
	double C;
	SpElem **x;
	int *y;
	int M;
	double eps;
	const SVM_Input *svmInput;

	KernelMatrix *Q;

	double *alpha;
	enum STATUS{ UPPER, LOWER, OTHER };
	int *status_alpha;
	double *G;				// Gradient of f(x)

	bool selectWorkSet( int &out_i, int &out_j );
	void saveModel( SVM_Model &model );
	// update status_alpha
	inline void update( int i )
	{
		if ( alpha[i] <= BOUND_EPS )
			status_alpha[i] = LOWER;
		else if ( alpha[i] >= C - BOUND_EPS )
			status_alpha[i] = UPPER;
		else
			status_alpha[i] = OTHER;
	}
	inline bool isUpper( int i )
	{
		return status_alpha[i] == UPPER;
	}
	inline bool isLower( int i )
	{
		return status_alpha[i] == LOWER;
	}
};

#endif
