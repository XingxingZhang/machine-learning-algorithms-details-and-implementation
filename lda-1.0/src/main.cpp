#include <stdio.h>
#include <time.h>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;
#include "lda_wrapper.h"

Para para;
string trfile;
string modelfile;

bool parseCmdLine( int argc, char **argv )
{
	para.K = 100;
	para.T = 2000;
	para.burnInT = 200;
	para.alpha = 0;
	para.beta = 0.1;
	int i;
	for( i = 1; i < argc; i += 2 )
	{
		if( argv[i][0] != '-' )
			break;
		if ( i + 1 >= argc )
		{
			fprintf(stderr,"option %s with no value\n", argv[i]);
			return false;
		}
		if ( !strcmp(argv[i],"-K") )
			para.K = atoi(argv[i+1]);
		else if ( !strcmp(argv[i],"-T") )
			para.T = atoi(argv[i+1]);
		else if ( !strcmp(argv[i],"-B") )
			para.burnInT = atoi(argv[i+1]);
		else if ( !strcmp(argv[i],"-alpha") )
			para.alpha = atof(argv[i+1]);
		else if ( !strcmp(argv[i],"-beta") )
			para.beta = atof(argv[i+1]);
		else
		{
			fprintf( stderr, "invalid option %s\n",argv[i]);
			return false;
		}
	}
	if ( i != argc - 2 )
		return false;
	trfile = argv[i];
	modelfile = argv[i+1];

	if( para.alpha == 0 )
		para.alpha = (double)50 / para.K;

	return true;
}

void printUsage()
{
	fprintf( stderr, 
			"Latent Dirichlet Allocation (LDA) by Xingxing Zhang\n"
			"lda [-options] <train_file> <model_name>\n"
			"options:\n"
			"  -K <int>\ttopic number (default 100)\n"
			"  -T <int>\tsampling iteration times (default 2000)\n"
			"  -B <int>\tburn in iterations (default 200)\n"
			"  -alpha <float>\thyper-parameter of theta (doc-topic distributions) (default 50/K)\n"
			"  -beta <float>\thyper-parameter of beta (topic-word distributions) (default 0.1)\n"
			);
}

void runLDA()
{
	clock_t start = clock(), end;
	LDAWrapper lda(&para);
	lda.readDocs(trfile.c_str());
	fprintf(stderr,"load train file done!\n"
			"time spend %lf seconds!\n",(double)(clock()-start)/CLOCKS_PER_SEC);
	lda.estimatePara(modelfile.c_str());
	fprintf(stderr,"estimate done! time spend %lf seconds!\n",(double)(clock()-start)/CLOCKS_PER_SEC);
}

int main( int argc, char **argv )
{
	if( !parseCmdLine(argc,argv) )
	{
		printUsage();
		return 1;
	}
	runLDA();
	
	return 0;
}
