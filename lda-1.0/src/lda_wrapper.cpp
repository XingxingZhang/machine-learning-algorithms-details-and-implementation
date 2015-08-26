#include "lda_wrapper.h"

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;
#include "iosutil.h"

LDAWrapper::LDAWrapper( const Para* _para )
	: para(_para)
{
	doc = NULL;
	ndoc = NULL;
}

LDAWrapper::~LDAWrapper()
{
	int i;
	if ( doc != NULL )
	{
		for( i = 0; i < M; i ++ )
			delete []doc[i];
		delete []doc;
	}
	if ( ndoc != NULL )
		delete []ndoc;
}

void LDAWrapper::readDocs( const char *trfile )
{
	iosutil os;
	FILE *fin = fopen(trfile,"r");
	if( fin == NULL )
	{
		fprintf( stderr, "load trfile %s failed!\n", trfile );
		return;
	}
	
	fscanf(fin,"%d\n",&M);
	doc = new int*[M];
	ndoc = new int[M];
	int i = 0, j;
	while( os.getline(fin) != NULL )
	{
		os.start();
		vector<int> tokens;
		const char *word = NULL;
		while( (word=os.token(" \t\r\n")) != NULL )
		{
			int id = wordID.add(word);
			tokens.push_back(id);
		}
		doc[i] = new int[tokens.size()];
		ndoc[i] = tokens.size();
		for( j = 0; j < (int)tokens.size(); j ++ )
			doc[i][j] = tokens[j];
		i ++;
	}

	fclose(fin);
}

void LDAWrapper::estimatePara( const char *modelfile )
{
	LDA lda(para,M,wordID.size(),doc,ndoc);
	lda.estimate();
	double **theta = lda.getTheta();
	double **phi = lda.getPhi();
	saveModel(modelfile,theta,phi);
}

void LDAWrapper::saveModel( const char *modelfile, double **theta, double **phi )
{
	int m, k, t, V = wordID.size(),
		K = para->K;
	string dictfile, phifile, thetafile;
	dictfile = phifile = thetafile = modelfile;
	dictfile += ".dict";
	phifile += ".phi";
	thetafile += ".theta";
	
	// save dict
	FILE *fd = fopen(dictfile.c_str(),"w");
	wordID.print(fd);
	fclose(fd);

	// save phi matrix
	FILE *fp = fopen(phifile.c_str(),"w");
	for( k = 0; k < K; k ++ )
	{
		for( t = 0; t < V; t ++ )
			fprintf(fp,"%lf ",phi[k][t] );
		fprintf(fp,"\n");
	}
	fclose(fp);

	// save theta matrix
	FILE *ft = fopen(thetafile.c_str(),"w");
	for( m = 0; m < M; m ++ )
	{
		for( k = 0; k < K; k ++ )
			fprintf(ft,"%lf ",theta[m][k]);
		fprintf(ft,"\n");
	}
	fclose(ft);
}
