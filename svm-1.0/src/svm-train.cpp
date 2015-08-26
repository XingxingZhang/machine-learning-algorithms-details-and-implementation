#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "SVM.h"

SVM_Input svmInput;
SVM_Model svmModel;
SpElem **x;
int *y;
int M;
SpElem *xStore;
int totStore;
char trainFile[512];
char modelFile[512];

void printUsage();
void parseCmdLine( int argc, char **argv );
void readTrainFile( const char *infile );
inline bool isSpace( int ch ) {	return ch == ' ' || ch == '\t';	}
inline bool isLineEnd( int ch ) {	return ch == '\n' || ch == EOF; }

int main( int argc, char **argv )
{
	parseCmdLine( argc, argv );
	readTrainFile( trainFile );

	SVM::train( svmInput, svmModel );
	SVM::save( modelFile, svmModel );

	delete []x;
	delete []y;
	delete []xStore;
	
	return 0;
}

void printUsage()
{
	printf
	( 
	"Usage: svm-train [options] <train_file> <model_file>\n"
	"options:\n"
	"-c cost : the C in c-svc (default 1)\n"
	"-t kernel_type : (default 2)\n"
	"	0 -- linear\n"
	"	1 -- polynomial : ( gama*x'*y + coef0 )^degree\n"
	"	2 -- radial basis function : exp( -gama*|x-y|^2)\n"
	"	3 -- sigmoid : tanh( gama*x'*y + coef0 )\n"
	"-d degree : degree in polynomial kernel (default 3)\n"
	"-g gama : gama in ploynomial, rbf, and sigmoid kernel (default 1/k, k = the demension of train vectors)\n"
	"-r coef0 : coef0 in polynomial and sigmoid kernel (default 0)\n"
	"-m cache size : cache memory size in MB (default 40)\n"
	"-e epsilon : the epsilon for termination (default 0.001)\n"
	);
}

void parseCmdLine( int argc, char **argv )
{
	int i;
	svmInput.C = 1;
	svmInput.kernelType = RBF;
	svmInput.degree = 3;
	svmInput.gama = 0;
	svmInput.coef0 = 0;
	svmInput.cacheSize = 40;
	svmInput.eps = 0.001;
	for( i = 1; i < argc; i += 2 )
	{
		if ( argv[i][0] != '-' )
			break;
		switch( argv[i][1] )
		{
		case 'c':
			svmInput.C = atof( argv[i+1] );
			break;
		case 't':
			svmInput.kernelType = atoi( argv[i+1] );
			break;
		case 'd':
			svmInput.degree = atof( argv[i+1] );
			break;
		case 'g':
			svmInput.gama = atof( argv[i+1] );
			break;
		case 'r':
			svmInput.coef0 = atof( argv[i+1] );
			break;
		case 'm':
			svmInput.cacheSize = atof( argv[i+1] );
			break;
		case 'e':
			svmInput.eps = atof( argv[i+1] );
			break;
		default:
			fprintf( stderr, "invalid option %s\n", argv[i] );
			break;
		}
	}
	if ( i != argc - 2 )
	{
		printUsage();
		exit(0);
	}
	snprintf( trainFile, sizeof(trainFile), "%s", argv[i] );
	snprintf( modelFile, sizeof(modelFile), "%s", argv[i+1] );
}

void readTrainFile( const char *infile )
{
	int i;
	FILE *in = fopen(infile,"r");
	if ( in == NULL )
	{
		fprintf( stderr, "open train file %s failed!\n", infile );
		exit(0);
	}
	int ch;
	M = 0;
	totStore = 0;
	while( (ch = fgetc(in)) != EOF )
	{
		if ( ch == '\n' )
			M ++;
		else if ( ch == ':' )
			totStore ++;
	}
	x = new SpElem*[M];
	y = new int[M];
	totStore += M;
	xStore = new SpElem[totStore];

	rewind(in);
	int x_cnt = 0, k, max_k = 0;
	for( i = 0; i < M; i ++ )
	{
		fscanf( in, "%d", &y[i] );
		k = 0;
		x[i] = xStore + x_cnt;
		while( true )
		{
			ch = fgetc(in);
			while( !isLineEnd(ch) && !isdigit(ch) )
				ch = fgetc(in);
			if ( isLineEnd(ch) )
				break;
			ungetc( ch, in );
			fscanf( in, "%d:%lf", &xStore[x_cnt].index, &xStore[x_cnt].value );
			x_cnt ++;
			k ++;
		}
		xStore[x_cnt].index = -1;
		xStore[x_cnt].value = 0;
		x_cnt ++;
		max_k = max2(max_k,k);
	}
	svmInput.M = M;
	svmInput.gama = (double)1 / max_k;
	svmInput.x = x;
	svmInput.y = y;

	fclose(in);
}
