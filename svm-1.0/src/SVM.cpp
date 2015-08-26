#include "SVM.h"
#include <ctype.h>

void SVM::train( const SVM_Input &input, SVM_Model &outModel )
{
	SVM_Solver solver( &input );
	solver.solve( outModel );
}

double SVM::predict( const SVM_Model &m, const SpElem *x )
{
	double sum = 0;
	for( int i = 0; i < m.nsv; i ++ )
		sum += m.alpha_y[i] * KUtil::kernel(&m,m.sv[i],x);
	return sum + m.b;
}

bool SVM::save( const char *outfile, const SVM_Model &model )
{
	const char* kernelMap[] = { "LINEAR", "POLY", "RBF", "SIGMOID" };
	FILE *out = fopen( outfile, "w" );
	if ( out == NULL )
	{
		fprintf( stderr, "save model : open outfile %s failed!\n", outfile  );
		return false;
	}
	int i, j;
	fprintf(out,"nsv=%d\n",model.nsv);
	
	for ( i = 0; i < model.nsv; i ++ )
	{
		fprintf( out, "%.16g", model.alpha_y[i] );
		for( j = 0; model.sv[i][j].index != -1; j ++ )
			fprintf( out, " %d:%.8g", model.sv[i][j].index, model.sv[i][j].value );
		fputc('\n',out);
	}
	fprintf(out,"b=%g\n",model.b);
	int type = model.kernelType;
	fprintf( out, "kernelType=%s=%d\n", kernelMap[type], type );
	fprintf( out, "degree=%g\n", model.degree );
	fprintf( out, "gama=%g\n", model.gama );
	fprintf( out, "coef0=%g\n", model.coef0 );

	fclose(out);
	return true;
}

bool SVM::load( const char *infile, SVM_Model &m, SpElem *&xspace )
{
	FILE *in = fopen( infile, "r" );
	if ( in == NULL )
	{
		fprintf( stderr, "open model file %s failed!\n", infile );
		return false;
	}

	fscanf( in, "nsv=%d", &m.nsv );
	int ch;
	int totVecSpace = 0;
	while( (ch=fgetc(in)) != EOF )
		if ( ch == ':' )
			totVecSpace ++;
	totVecSpace += m.nsv;
	rewind(in);

	m.sv = new SpElem*[m.nsv];
	m.alpha_y = new double[m.nsv];
	SpElem *xSpace = new SpElem[totVecSpace];
	int x_cnt = 0;
	fscanf( in, "nsv=%d", &m.nsv );
	for( int i = 0; i < m.nsv; i ++ )
	{
		fscanf( in, "%lf", &m.alpha_y[i] );
		m.sv[i] = xSpace + x_cnt;
		while( true )
		{
			ch = fgetc(in);
			while( ch != '\n' && !isdigit(ch) )
				ch = fgetc(in);
			if ( ch == '\n' )
				break;
			ungetc(ch,in);
			fscanf(in,"%d:%lf", &xSpace[x_cnt].index, &xSpace[x_cnt].value);
			x_cnt ++;
		}
		xSpace[x_cnt].index = -1;
		xSpace[x_cnt].value = 0;
		x_cnt ++;
	}
	char buf[64];
	fscanf( in, "b=%lf", &m.b );
	fscanf( in, "kernelType=%s=%d", buf, &m.kernelType );
	fscanf( in, "degree=%lf", &m.degree );
	fscanf( in, "gama=%lf", &m.gama );
	fscanf( in, "coef0=%lf", &m.coef0 );

	xspace = xSpace;

	fclose( in );
	return true;
}
