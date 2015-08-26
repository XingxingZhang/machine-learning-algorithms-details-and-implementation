#include "lda.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <iostream>
using namespace std;

LDA::LDA( const Para *para, int _M, int _V, int **_doc, int *_ndoc )
{
	K = para->K;
	T = para->T;
	burnInT = para->burnInT;
	alpha = para->alpha;
	beta = para->beta;
	M = _M;
	V = _V;
	doc = _doc;
	ndoc = _ndoc;
	z = NULL;
	Nmz = NULL;
	Nm = NULL;
	Nzt = NULL;
	Nz = NULL;
	theta = NULL;
	phi = NULL;
}

LDA::~LDA()
{
	int i;
	if ( z != NULL )
	{
		for( i = 0; i < M; i ++ )
			delete []z[i];
		delete []z;
	}
	if ( Nmz != NULL )
	{
		for( i = 0; i < M; i ++ )
			delete []Nmz[i];
		delete []Nmz;
	}
	if ( Nm != NULL )
		delete []Nm;
	if ( Nzt != NULL )
	{
		for( i = 0; i < K; i ++ )
			delete []Nzt[i];
		delete []Nzt;
	}
	if ( Nz != NULL )
		delete []Nz;
	if ( theta != NULL )
	{
		for( i = 0; i < M; i ++ )
			delete []theta[i];
		delete []theta;
	}
	if ( phi != NULL )
	{
		for( i = 0; i < K; i ++ )
			delete []phi[i];
		delete []phi;
	}
}

void LDA::init()
{
	int m, n, k, t;
	theta = new double*[M];
	phi = new double*[K];
	Nmz = new int*[M];
	Nm = new int[M];
	Nzt = new int*[K];
	Nz = new int[K];
	memset( Nm, 0, sizeof(int)*M );
	memset( Nz, 0, sizeof(int)*K );
	for( m = 0; m < M; m ++ )
	{
		theta[m] = new double[K];
		Nmz[m] = new int[K];
		for( k = 0; k < K; k ++ )
		{
			theta[m][k] = 0;
			Nmz[m][k] = 0;
		}
	}
	for( k = 0; k < K; k ++ )
	{
		phi[k] = new double[V];
		Nzt[k] = new int[V];
		for( t = 0; t < V; t ++ )
		{
			phi[k][t] = 0;
			Nzt[k][t] = 0;
		}
	}

	// random assign topics
	z = new int*[M];
	for( m = 0; m < M; m ++ )
	{
		z[m] = new int[ndoc[m]];
		for( n = 0; n < ndoc[m]; n ++ )
		{
			int rz = rand() % K;
			z[m][n] = rz;
			Nmz[m][rz] ++;
			Nm[m] ++;
			Nzt[rz][doc[m][n]] ++;
			Nz[rz] ++;
		}
	}
}

/**
 * find such i in arr that arr[i-1] < key <= arr[i]
 */
int LDA::binaryle( double *arr, int begin, int end, double key )
{
	int maxIndex = end;
	while( begin <= end )
	{
		int mid = (begin+end) >> 1;
		if ( key < arr[mid] )
			end = mid - 1;
		else if ( key > arr[mid] )
			begin = mid + 1;
		else
			return mid;
	}
	if ( begin > maxIndex )
		return maxIndex;
	else
		return begin;
}

void LDA::estimate()
{
	init();
	
	// p(z_j=k|vec(z_-j),vec(w))
	double *P_zj = new double[K];
	double *sum = new double[K+1];
	int i, m, n, k, t, cz, w;
	
	srand(time(0));
	validSampleT = 0;
	double K_alpha = M * alpha;
	double V_beta = V * beta;
	for( i = 0; i < T; i ++ )
	{
		// sampling each z[m][n]
		for( m = 0; m < M; m ++ )
			for( n = 0; n < ndoc[m]; n ++ )
			{
				// subtract z[m][n] and w[m][n]
				cz = z[m][n];
				w = doc[m][n];
				Nmz[m][cz] --;
				Nm[m] --;
				Nzt[cz][w] --;
				Nz[cz] --;

				// caculate  p(z_j=k|vec(z_not_j),vec(w))
				sum[0] = 0;
				for ( k = 0; k < K; k ++ )
				{
					P_zj[k] = (Nmz[m][k] + alpha)*(Nzt[k][w]+beta)
						/(Nm[m]+K_alpha)/(Nz[k]+V_beta);
					sum[k+1] = sum[k] + P_zj[k];
				}

				// sample k
				double rnd = (double)rand() / RAND_MAX * sum[K];
				k = binaryle(sum,1,K,rnd) - 1;
				
				// recover z[m][n] and w[m][n]
				z[m][n] = k;
				Nmz[m][k] ++;
				Nm[m] ++;
				Nzt[k][w] ++;
				Nz[k] ++;
			}

		// caculate theta and phi
		if ( i >= burnInT )
		{
			double Dir_sum;
			for( m = 0; m < M; m ++ )
			{
				Dir_sum = Nm[m] + K_alpha;
				for( k = 0; k < K; k ++ )
					theta[m][k] += double(Nmz[m][k] + alpha)/Dir_sum;
			}
			for( k = 0; k < K; k ++ )
			{
				Dir_sum = Nz[k] + V_beta;
				for( t = 0; t < V; t ++ )
					phi[k][t] += double(Nzt[k][t]+beta)/Dir_sum;
			}

			validSampleT ++;
		}
		if ( (i+1) % 10 == 0 )
			fprintf(stderr,"iter = %d\n", i + 1 );
	}
	delete []P_zj;
	delete []sum;

	// caculate final theta and phi
	for( m = 0; m < M; m ++ )
		for( k = 0; k < K; k ++ )
			theta[m][k] /= validSampleT;
	for( k = 0; k < K; k ++ )
		for( t = 0; t < V; t ++ )
			phi[k][t] /= validSampleT;
}
