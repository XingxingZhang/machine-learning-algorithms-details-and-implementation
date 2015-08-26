#ifndef __MAXENT_H__
#define __MAXENT_H__

#include <string>
#include <vector>
using namespace std;

#include "strid.h"

struct SpElem
{
	int index;
	double value;
};

/**
 * A maximum entropy model
 */
class MEModel
{
public:
	MEModel() : needDestroyTrain(false), needDestroyLambda(false) {}
	bool loadTrainingExamples( const char *trainFile );
	void print();
	bool loadModel( const char *modelFile );
	bool saveModel( const char *modelFile );
	bool modelBuildDone()	{	return needDestroyLambda;	}
	bool provideTrainDataStruct( SpElem **&x, int *&y, int &M, int &classNumber, int &featureSize, double *&_lambda );
	bool getFeaId( const string fea, int &id )
	{
		return feaIdMap.get(fea,id);
	}
	bool getClsName( const int clsNum, string &clsName )
	{
		return classNameIdMap.get(clsNum,clsName);
	}
	~MEModel();
private:
	int classNumber;
	StrId classNameIdMap;
	double *lambda;
	int featureSize;
	StrId feaIdMap;

	SpElem **x;
	SpElem *xspace;
	int *y;
	int M;
	bool needDestroyTrain;
	bool needDestroyLambda;

	bool isLineEnd( int ch ) {	return ch == '\n' || ch == EOF;	}
	bool isSpace( int ch ) {	return ch == ' ' || ch == '\t' || ch == '\r';	}
};

/**
 * A maximum entropy classifier
 */
class MaxEnt
{
public:
	MaxEnt() : epsilon(1.0E-05) {	model = new MEModel;	}
	bool loadTrainingExamples( const char *infile );
	bool saveModel( const char *outfile );
	bool loadModel( const char *infile );
	bool train( int iter, double expPrior );
	int predict( vector<string> feature, vector<pair<string,double> > &labelProbs );
	int predict( const char *testFile, const char *predictFile, bool withClassLabel );
	~MaxEnt()	{	delete model;	}
private:
	MEModel *model;
	SpElem **x;
	int *y;
	int M;
	double *lambda;
	int classNumber;
	int featureSize;
	const double epsilon;

	bool isSpace( int ch ) {	return ch == ' ' || ch == '\t' || ch == '\r';	}

	/**
	 * get observed expectations of all features. 
	 * E_obs(f_i) = sum {p_obs(x,y)*f_i(x,y)}
	 * return C, which is the maximum observed expectation.
	 */
	double getFeaObsCounts( double *obsCounts );
	/**
	 * compute current expectations of all features
	 * according to the current lambda
	 * return log_Likelihood
	 */
	double getFeaExpects( double *expects );
	/**
	 * get p(y|vec), y = 0, 1, ..., classNumber - 1
	 */
	void getProbs( SpElem *vec, double *probs );

	/**
	 * input p1, p2, ..., pn
	 * return log ( exp(p1) + exp(p2) + ... + exp(pn) )
	 */
	double sumLogExp( double *probs, int size );

	bool parseLine( FILE *fin, FILE *fout, bool withClassLabel );
};

#endif
