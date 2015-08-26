#ifndef __KERNEL_MATRIX_H__
#define __KERNEL_MATRIX_H__

#include <stdlib.h>
#include <math.h>
#include "SVM_Type.h"
#include "Cache.h"

#include <stdio.h>

class KUtil
{
public:
	static double dot( const SpElem *v1, const SpElem *v2 );
	static double rbf( const double gama, const SpElem *v1, const SpElem *v2 );
	static double kernel( const SVM_Model *model, const SpElem *v1, const SpElem *v2 );
};

class Kernel
{
public:
	virtual double kernel( const SVM_Input *input, int i, int j ) const = 0;
};

class LinearKernel : public Kernel
{
public:
	double kernel( const SVM_Input *input, int i, int j ) const
	{	return KUtil::dot(input->x[i],input->x[j]);	}
};

class PolyKernel : public Kernel
{
public:
	double kernel( const SVM_Input *input, int i, int j ) const
	{	return pow(input->gama*KUtil::dot(input->x[i],input->x[j])+input->coef0,input->degree);	}
};

class RBFKernel : public Kernel
{
public:
	double kernel( const SVM_Input *input, int i, int j ) const
	{
		return KUtil::rbf(input->gama,input->x[i],input->x[j]);
	}
};

class SigmoidKernel : public Kernel
{
public:
	double kernel( const SVM_Input *input, int i, int j ) const
	{
		return tanh(input->gama*KUtil::dot(input->x[i],input->x[j])+input->coef0);
	}
};

class KernelMatrix
{
public:
	KernelMatrix( const SVM_Input *_input );
	double* getKernelMatrixRow( int i ) const;
	~KernelMatrix();
private:
	Cache *cache;
	int M;
	const SVM_Input* input;
	Kernel *kernel;
};

#endif
