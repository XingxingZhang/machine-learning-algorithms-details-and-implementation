#ifndef __SVM_TYPE_H__
#define __SVM_TYPE_H__


// the element in a sparse vector
struct SpElem
{
	int index;
	double value;
};

enum KernelType{ LINEAR, POLY, RBF, SIGMOID };

struct SVM_Input
{
	double C;			// C in C-SVC
	SpElem **x;			// training data vectors (sparse vector)
	int *y;				// training data lebels
	int M;				// training data size
	double eps;			// epsilon for termination
	int kernelType;
	// parameters in kernel functions
	double degree;
	double gama;
	double coef0;

	double cacheSize;	// cache size in MB
};

class SVM_Model
{
public:
	double b;
	int nsv;			//  num of support vectors
	SpElem **sv;		// support vectors
	double *alpha_y;	// alpha[i]*y[i] i = 0,...,nsv-1
	int kernelType;
	double degree;
	double gama;
	double coef0;

	~SVM_Model()
	{
		delete []sv;
		delete []alpha_y;
	}
};

#endif
