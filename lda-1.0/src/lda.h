#ifndef __LDA_H__
#define __LDA_H__

struct Para
{
	int K;			// topic number
	int T;			// sampling number
	int burnInT;	// burn in times
	double alpha;	// hyper-parameter of theta
	double beta;	// hyper-parameter of phi
};

class LDA
{
public:
	LDA( const Para *para, int _M, int _V, int **_doc, int *_ndoc );
	void estimate();
	double **getTheta()	{	return theta;	}
	double **getPhi()	{	return phi;	}
	~LDA();
private:
	int K;			// topic number
	int T;			// sampling times
	int burnInT;
	int validSampleT;	// the times after burn in period
	double alpha;
	double beta;

	int M;			// doc number
	int V;			// vacabulary size
	int **doc;		// documents
	int *ndoc;		// doc length

	int **z;
	int **Nmz;
	int *Nm;
	int **Nzt;
	int *Nz;
	double **theta;
	double **phi;

	void init();
	int binaryle( double *arr, int start, int end, double key );
};

#endif
