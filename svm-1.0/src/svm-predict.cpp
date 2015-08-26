#include <stdio.h>
#include <ctype.h>

#include "SVM.h"

char testFile[512];
char modelFile[512];
char predictFile[512];
SVM_Model svmModel;

class X
{
public:
	X( int _size = 1024 ) : size(_size)
	{
		x = new SpElem[size];
		curIndex = 0;
	}
	void init() {	curIndex = 0;	}
	void add( int index, double value )
	{
		if ( curIndex >= size )
			resize( size << 1 );
		x[curIndex].index = index;
		x[curIndex].value = value;
		curIndex ++;
	}
	SpElem *getX()	{	return x;	}
	bool isEmpty()	{	return curIndex == 0;	}
	~X() {	delete []x;	}
private:
	SpElem *x;
	int size;
	int curIndex;

	void resize( int new_size )
	{
		SpElem *tmp = new SpElem[new_size];
		for( int i = 0; i < size; i ++ )
			tmp[i] = x[i];
		delete []x;
		x = tmp;
	}
};

void printUsage();
void predict( const char *infile, const char *outfile );

int main( int argc, char **argv )
{
	if ( argc != 4 )
	{
		printUsage();
		return 1;
	}
	snprintf( testFile, sizeof(testFile), "%s", argv[1] );
	snprintf( modelFile, sizeof(modelFile), "%s", argv[2] );
	snprintf( predictFile, sizeof(predictFile), "%s", argv[3] );
	SpElem *xspace = NULL;
	SVM::load( modelFile, svmModel, xspace );
	printf( "load model done!\n" );
	predict( testFile, predictFile );
	delete []xspace;

	return 0;
}

void printUsage()
{
	printf( "Usage: svm-predict <test-file> <model-file> <predict-file>" );
}

void predict( const char *infile, const char *outfile )
{
	FILE *in = fopen(infile,"r");
	FILE *out = fopen(outfile,"w");
	bool isEndOfFile = false;
	int y;
	X x;
	int tot = 0, correct = 0;
	while( !isEndOfFile && fscanf(in, "%d", &y) != EOF )
	{
		x.init();
		while( true )
		{
			int ch = fgetc(in);
			while( ch != '\n' && ch != EOF && !isdigit(ch) )
				ch = fgetc(in);
			if ( ch == EOF )
			{
				isEndOfFile = true;
				break;
			}
			else if ( ch == '\n' )
				break;
			ungetc(ch,in);
			int index;
			double value;
			fscanf( in, "%d:%lf", &index, &value );
			x.add( index, value );
		}
		x.add( -1, 0 );
		double p = SVM::predict(svmModel,x.getX());
		int py = p >= 0 ? 1 : -1;
		fprintf(out,"%g\n",p);
		if ( py == y )
			correct ++;
		tot ++;
	}
	printf( "precision = %lf (%d/%d)\n", (double)correct/tot, correct, tot );
	fclose(in);
	fclose(out);
}
