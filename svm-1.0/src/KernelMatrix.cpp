#include "KernelMatrix.h"

double KUtil::dot( const SpElem *v1, const SpElem *v2 )
{
	double dotp = 0;
	int i = 0, j = 0;
	while( v1[i].index != -1 && v2[j].index != -1 )
	{
		if( v1[i].index < v2[j].index )
			i ++;
		else if( v2[j].index < v1[i].index )
			j ++;
		else
		{
			dotp += v1[i].value * v2[j].value;
			i ++; j ++;
		}
	}
	return dotp;
}

double KUtil::rbf( double gama, const SpElem *v1, const SpElem *v2 )
{
	double sum = 0;
	int i = 0, j = 0;
	while( v1[i].index != -1 && v2[j].index != -1 )
	{
		if( v1[i].index < v2[j].index )
		{
			sum += v1[i].value * v1[i].value;
			i ++;
		}
		else if( v2[j].index < v1[i].index )
		{
			sum += v2[j].value * v2[j].value;
			j ++;
		}
		else
		{
			double tmp = v1[i].value - v2[j].value;
			sum += tmp * tmp;
			i ++; j ++;
		}
	}
	while( v1[i].index != -1 )
	{
		sum += v1[i].value*v1[i].value;
		i ++;
	}
	while( v2[j].index != -1 )
	{
		sum += v2[j].value*v2[j].value;
		j ++;
	}
	return exp(-gama*sum);
}

double KUtil::kernel( const SVM_Model *m, const SpElem *v1, const SpElem *v2 )
{
	if ( m->kernelType == LINEAR )
		return dot( v1, v2 );
	else if ( m->kernelType == POLY )
		return pow(m->gama*dot(v1,v2)+m->coef0,m->degree);
	else if ( m->kernelType == RBF )
		return rbf(m->gama,v1,v2);
	else if ( m->kernelType == SIGMOID )
		return tanh(m->gama*dot(v1,v2)+m->coef0);
	else
	{
		fprintf( stderr, "invalid kernel type in KUtil\n" );
		return 0.0;
	}
}

KernelMatrix::KernelMatrix( const SVM_Input *_input )
	: input(_input), M(_input->M)
{
	int size = (int)(input->cacheSize * (1<<20) / (M*sizeof(double)));
	size = M < size ? M : size;
	cache = new Cache(M,size);
	int kernelType = input->kernelType;
	if ( kernelType == LINEAR )
		kernel = new LinearKernel();
	else if ( kernelType == POLY )
		kernel = new PolyKernel();
	else if ( kernelType == RBF )
		kernel = new RBFKernel();
	else if ( kernelType == SIGMOID )
		kernel = new SigmoidKernel();
	else
	{
		fprintf( stderr, "invalid kernel type in class KernelMatrix\n" );
		exit(0);
	}
}
double* KernelMatrix::getKernelMatrixRow( int i ) const
{
	double *row = NULL;
	if ( cache->getData( i, row ) )
		return row;
	for( int k = 0; k < M; k ++ )
		row[k] = kernel->kernel(input,i,k)*input->y[i]*input->y[k];
	return row;
}
KernelMatrix::~KernelMatrix()
{
	delete cache;
	delete kernel;
}
