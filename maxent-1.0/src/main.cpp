#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "maxent.h"

int iter;
double expPrior;
char trainFile[1024];
char testFile[1024];
char predictFile[1024];

void printUsage()
{
	fprintf( stderr, 
	"maxent [optionis] <trainFile> <testFile> <predictFile>\n"
	"options:\n"
	"\t-iINT\tnumber of iterations, default 100\n"
	"\t-pFLOAT\texponential prior, default 0.1\n"
	);
}

bool readCmdLine( int argc, char **argv )
{
	iter = 100;
	expPrior = 0.1;
	int i;
	for( i = 1; i < argc; i ++ )
	{
		if( strcmp(argv[i],"i") == 0 || strcmp(argv[i],"-i") == 0 )
		{
			if ( i + 1 < argc )
			{
				iter = atoi(argv[i+1]);
				i ++;
			}
			else
				return false;
		}
		else if ( strcmp(argv[i],"p") == 0 || strcmp(argv[i],"-p") == 0 )
		{
			if ( i + 1 < argc )
			{
				expPrior = atof(argv[i+1]);
				i ++;
			}
			else
				return false;
		}
		else
			break;
	}
	if ( argc - i != 3 )
		return false;

	snprintf( trainFile, sizeof(trainFile), argv[i] );
	snprintf( testFile, sizeof(testFile), argv[i+1] );
	snprintf( predictFile, sizeof(predictFile), argv[i+2] );

	return true;
}
void runMaxEnt()
{
	MaxEnt me;
	me.loadTrainingExamples(trainFile);
	me.train(iter,expPrior);
	me.predict(testFile,predictFile,true);
}

int main( int argc, char **argv )
{
	if (!readCmdLine( argc, argv ))
	{
		printUsage();
		return 1;
	}
	// printf( "i = %d, p = %lf, train = %s, test = %s, predict = %s\n", iter, expPrior, trainFile, testFile, predictFile );
	runMaxEnt();

	return 0;
}

