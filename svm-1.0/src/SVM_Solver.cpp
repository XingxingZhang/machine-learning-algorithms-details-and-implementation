#include "SVM_Solver.h"

#include <stdio.h>
#include <float.h>

SVM_Solver::SVM_Solver( const SVM_Input *input )
	: C(input->C), x(input->x), y(input->y), M(input->M), eps(input->eps), svmInput(input)
{
	Q = new KernelMatrix(input);
	alpha = new double[M];
	status_alpha = new int[M];
	G = new double[M];
}

SVM_Solver::~SVM_Solver()
{
	delete Q;
	delete []alpha;
	delete []status_alpha;
	delete []G;
}

/**
 * Solve min Grad(f(alpha))^T * d
 *       s.t. y^T * d = 0, -1 <= d[i] <= 1
 *            d[i] >= 0, if alpha[i] = 0
 *            d[i] <= 0, if alpha[i] = C
 *            |{d[i]|d[i]!=0}| = 2
 *
 * The solution is (details in LIBSVM: Introduction and Benchmarks):
 * i = argmin( { Grad(f(alpha))_i | y[i]=1, 0<=alpha[i]<C} ,
 *             { -Grad(f(alpha))_i | y[i]=-1, 0<alpha[i]<=C } )
 * j = argmin( { -Grad(f(alpha))_i | y[i]=1, 0<alpha[i]<=C} ,
 *             { Grad(f(alpha))_i | y[i]=-1, 0<=alpha[i]<C } )
 */
bool SVM_Solver::selectWorkSet( int &out_i, int &out_j )
{
	double min_O_i = DBL_MAX, min_O_j = DBL_MAX;
	
	int min_i = -1, min_j = -1;
	for( int i = 0; i < M; i ++ )
	{
		if ( y[i] == 1 )
		{
			if ( !isUpper(i) )
				min_rep( G[i], min_O_i, i, min_i );
			if ( !isLower(i) )
				min_rep( -G[i], min_O_j, i, min_j );
		}
		else
		{
			if ( !isLower(i) )
				min_rep( -G[i], min_O_i, i, min_i );
			if ( !isUpper(i) )
				min_rep( G[i], min_O_j, i, min_j );
		}
	}
	if ( -min_O_i - min_O_j < eps )
		return false;
	
	out_i = min_i;
	out_j = min_j;

	return true;
}

void SVM_Solver::saveModel( SVM_Model &model )
{
	int i, j;
	// calculate b
	// if kkt condition is statisfied, then
	// if 0 < alpha[i] < C, y[i]*(w^T*x[i] + b) = 1, 
	// then b = y[i] - w^T*x[i] = -y[i]*G[i]
	double sum = 0.0;
	double free_cnt = 0;
	double ub = -DBL_MAX, lb = DBL_MAX;
	int nsv = 0;
	for( i = 0; i < M; i ++ )
	{
		double tmp = -y[i]*G[i];
		if ( !isLower(i) )
		{
			nsv ++;
			if ( !isUpper(i) )
			{
				free_cnt ++;
				sum += tmp;
			}
			else
				lb = min2(lb,tmp);
		}
		else
			ub = max2(ub,tmp);
	}
	double b;
	if ( free_cnt != 0 )
		b = sum / free_cnt;
	else
		b = ( ub + lb ) / 2;
	model.b = b;
	model.nsv = nsv;
	j = 0;
	model.sv = new SpElem*[nsv];
	model.alpha_y = new double[nsv];
	for( i = 0; i < M; i ++ )
		if ( !isLower(i) )
		{
			model.alpha_y[j] = y[i]*alpha[i];
			model.sv[j] = x[i];
			j ++;
		}
	model.kernelType = svmInput->kernelType;
	model.degree = svmInput->degree;
	model.gama = svmInput->gama;
	model.coef0 = svmInput->coef0;
}

void SVM_Solver::solve( SVM_Model &model )
{
	int i, j, k;
	for( i = 0; i < M; i ++ )
	{
		alpha[i] = 0;
		status_alpha[i] = LOWER;
		G[i] = -1;		// Grad(f(x)) = Q*alpha - 1
	}
	int iter = 0;
	while( selectWorkSet( i, j ) )
	{
		iter ++;
		double H, L;
		double *Q_i = Q->getKernelMatrixRow(i);
		double *Q_j = Q->getKernelMatrixRow(j);
		double alpha_i_old = alpha[i];
		double alpha_j_old = alpha[j];
		// details in SMO algorithm and 
		// LIBSVM: Introduction and Benchmarks
		if ( y[i] != y[j] )
		{
			L = max2(0.0,alpha[j]-alpha[i]);
			H = min2(C,C+alpha[j]-alpha[i]);
			alpha[j] += (-G[i]-G[j])/(Q_i[i]+Q_j[j]+2*Q_i[j]);
			if ( alpha[j] > H )
				alpha[j] = H;
			else if ( alpha[j] < L )
				alpha[j] = L;
			alpha[i] += alpha[j] - alpha_j_old;
		}
		else
		{
			L = max2(0.0,alpha[j]+alpha[i]-C);
			H = min2(C,alpha[j]+alpha[i]);
			alpha[j] += (G[i]-G[j])/(Q_i[i]+Q_j[j]-2*Q_i[j]);
			if ( alpha[j] > H )
				alpha[j] = H;
			else if ( alpha[j] < L )
				alpha[j] = L;
			alpha[i] -= alpha[j] - alpha_j_old;
		}
		// update G
		double delta_alpha_i = alpha[i] - alpha_i_old;
		double delta_alpha_j = alpha[j] - alpha_j_old;
		for( k = 0; k < M; k ++ )
			G[k] += delta_alpha_i * Q_i[k] + delta_alpha_j * Q_j[k];
		// update alpha[i] and alpha[j]
		update(i);
		update(j);
	}
	saveModel( model );
	printf( "save model done! iter = %d\n", iter );
}
