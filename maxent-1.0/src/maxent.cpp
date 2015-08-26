#include <stdio.h>
#include <math.h>
#include "maxent.h"
#include "varstr.h"

bool MEModel::loadTrainingExamples( const char *trainFile )
{
	FILE *fin = fopen(trainFile,"r");
	if ( fin == NULL )
	{
		fprintf( stderr, "open train file %s failed!\n", trainFile );
		return false;
	}
	int lineNum = 0;
	int fieldNum = 0;
	int ch;
	bool endOfFile = false;
	int curFieldCnt = 0;
	while( !endOfFile )
	{
		ch = fgetc(fin);
		curFieldCnt = 0;
		while( true )
		{
			while( !isLineEnd(ch) && isSpace(ch) )
				ch = fgetc(fin);
			if( isLineEnd(ch) )
				break;
			while( !isLineEnd(ch) && !isSpace(ch) )
				ch = fgetc(fin);
			curFieldCnt ++;
		}
		if ( curFieldCnt > 0 )
		{
			lineNum ++;
			fieldNum += curFieldCnt;
		}
		if( ch == EOF )
			endOfFile = true;
	}
	rewind(fin);
	M = lineNum;
	VarStr word;
	endOfFile = false;
	xspace = new SpElem[fieldNum];
	x = new SpElem*[M];
	y = new int[M];
	lineNum = 0;
	int xIndex = 0;
	while( !endOfFile )
	{
		ch = fgetc(fin);
		curFieldCnt = 0;
		int curY = 0, curX = 0;
		char *ystr = NULL;
		while( true )
		{
			while( !isLineEnd(ch) && isSpace(ch) )
				ch = fgetc(fin);
			if( isLineEnd(ch) )
				break;
			word.clear();
			while( !isLineEnd(ch) && !isSpace(ch) )
			{
				word.add(ch);
				ch = fgetc(fin);
			}
			curFieldCnt ++;
			if( curFieldCnt == 1 )
			{
				ystr = new char[word.length()+1];
				strcpy(ystr,word.c_str());
			}
			else
			{
				curX = feaIdMap.add(word.c_str());
				xspace[xIndex].index = curX;
				xspace[xIndex].value = 1;
				if ( curFieldCnt == 2 )
					x[lineNum] = &xspace[xIndex];
				xIndex ++;
			}
		}
		if ( curFieldCnt > 1 )
		{
			xspace[xIndex].index = -1; xspace[xIndex].value = 0;
			xIndex ++;
			curY = classNameIdMap.add(ystr);
			y[lineNum] = curY;
			lineNum ++;
		}
		if( ystr != NULL )
			delete []ystr;
		if ( ch == EOF )
			endOfFile = true;
	}
	
	fclose(fin);
	M = lineNum;
	classNumber = (int)classNameIdMap.size();

	needDestroyTrain = true;
	return true;
}

void MEModel::print()
{
	printf( "class\n" );
	classNameIdMap.print( stdout );
	printf( "class num = %d\n", classNumber );
	printf( "feature\n" );
	feaIdMap.print( stdout );
	for( int i = 0; i < M; i ++ )
	{
		printf( "y = %d, x = ", y[i] );
		for( int j = 0; x[i][j].index != -1; j ++ )
			printf( " %d", x[i][j].index );
		putchar( '\n' );
	}
}

bool MEModel::saveModel( const char *modelFile )
{
	if( !needDestroyLambda )
	{
		fprintf( stderr, "maxent model has not been trained!\n" );
		return false;
	}
	FILE *fout = fopen(modelFile,"w");
	if ( fout == NULL )
	{
		fprintf( stderr, "save maxent model failed! %s\n", modelFile );
		return false;
	}
	classNameIdMap.print(fout);
	feaIdMap.print(fout);
	fprintf( fout, "%d\n", featureSize );
	for( int i = 0; i < featureSize; i ++ )
		fprintf( fout, "%lf\n", lambda[i] );
	fclose(fout);

	return true;
}

bool MEModel::loadModel( const char *modelFile )
{
	FILE *fin = fopen(modelFile,"r");
	if( fin == NULL )
	{
		fprintf( stderr, "load maxent model file %s failed!\n", modelFile );
		return false;
	}
	classNameIdMap.read(fin);
	classNumber = classNameIdMap.size();
	feaIdMap.read(fin);
	fscanf( fin, "%d", &featureSize );
	for( int i = 0; i < featureSize; i ++ )
		fscanf( fin, "%lf", &lambda[i] );
	fclose(fin);
	needDestroyLambda = true;

	return true;
}

bool MEModel::provideTrainDataStruct( SpElem **&_x, int *&_y, int &_M, int &_classNumber, int &_featureSize, double *&_lambda )
{
	if ( !needDestroyTrain )
		return false;
	_x = x;
	_y = y;
	_M = M;
	_classNumber = classNumber;
	_featureSize = featureSize = feaIdMap.size() * classNumber;
	lambda = new double[featureSize];
	needDestroyLambda = true;
	_lambda = lambda;

	return true;
}

MEModel::~MEModel()
{
	if ( needDestroyTrain )
	{
		delete []xspace;
		delete []x;
		delete []y;
	}
	if ( needDestroyLambda )
		delete []lambda;
}

bool MaxEnt::loadTrainingExamples( const char* infile )
{
	return model->loadTrainingExamples(infile);
}

bool MaxEnt::saveModel( const char *outfile )
{
	return model->saveModel(outfile);
}

bool MaxEnt::loadModel( const char *infile )
{
	return model->loadModel(infile);
}

double MaxEnt::getFeaObsCounts( double *obsCounts )
{
	int i, j;
	for( i = 0; i < featureSize; i ++ ) obsCounts[i] = 0; 
	for( i = 0; i < M; i ++ )
	{
		int cls = y[i];
		SpElem *vec = x[i];
		for( j = 0; vec[j].index != -1; j ++ )
			obsCounts[vec[j].index*classNumber+cls] += vec[j].value;

	}
	double maxCnt = 0;
	for( i = 0; i < featureSize; i ++ )
		if (obsCounts[i]>maxCnt)
			maxCnt = obsCounts[i];

	return maxCnt;
}

double MaxEnt::sumLogExp( double *probs, int size )
{
	int i;
	double maxp = 0;
	for( i = 0; i < size; i ++ )
		if ( probs[i] > maxp )
			maxp = probs[i];
	double sum = 0;
	for( i = 0; i < size; i ++ )
		sum += exp(probs[i]-maxp);

	return maxp + log(sum);
}

void MaxEnt::getProbs( SpElem *vec, double *probs )
{
	int i, j;
	double *wtedFeaSum = new double[classNumber];
	
	for( i = 0; i < classNumber; i ++ ) wtedFeaSum[i] = 0;
	for( i = 0; vec[i].index != -1; i ++ )
		for( j = 0; j < classNumber; j ++ )
			wtedFeaSum[j] += lambda[vec[i].index*classNumber+j] * vec[i].value;
	double sum = sumLogExp( wtedFeaSum, classNumber );
	for( i = 0; i < classNumber; i ++ )
		probs[i] = exp(wtedFeaSum[i]-sum);

	delete []wtedFeaSum;
}

double MaxEnt::getFeaExpects( double *expects )
{
	int i, j, k;
	for( i = 0; i < featureSize; i ++ ) expects[i] = 0;
	double *probs = new double[classNumber];
	double log_Likelihood = 0;
	for( i = 0; i < M; i ++ )
	{
		SpElem *vec = x[i];
		getProbs( vec, probs );
		log_Likelihood += log(probs[y[i]]);
		for( j = 0; vec[j].index != -1; j ++ )
			for( k = 0; k < classNumber; k ++ )
				expects[vec[j].index*classNumber+k] += probs[k] * vec[j].value;
	}
	delete []probs;

	return log_Likelihood;
}

bool MaxEnt::train( int iter, double expPrior )
{
	if( !model->provideTrainDataStruct(x,y,M,classNumber,featureSize,lambda) )
	{
		fprintf( stderr, "train failed! -- training examples have not been loaded!\n" );
		return false;
	}
	int i, j;
	double previousLogLikelihood;
	double currentLogLikelihood;
	double *obsCounts = new double[featureSize];
	double *expectCounts = new double[featureSize];
	double C = getFeaObsCounts(obsCounts);
	
	for( i = 0; i < featureSize; i ++ ) lambda[i] = 0;
	for( i = 0; i < iter; i ++ )
	{
		currentLogLikelihood = getFeaExpects(expectCounts);
		if ( i != 0 && fabs(currentLogLikelihood - previousLogLikelihood) < epsilon )
			break;
		printf( "iter = %d, log likelihood = %lf, epsilion = %lf\n", i+1, currentLogLikelihood, fabs(currentLogLikelihood-previousLogLikelihood) );
		previousLogLikelihood = currentLogLikelihood;
		for( j = 0; j < featureSize; j ++ )
		{
			double nextLambda = 0;
			double tmp = obsCounts[j] - expPrior;
			if ( tmp > 0 )
				nextLambda = lambda[j] + log(tmp/expectCounts[j])/C;
			lambda[j] = nextLambda > 0 ? nextLambda : 0;
		}
	}

	delete []obsCounts;
	delete []expectCounts;

	for( i = 0; i < featureSize; i ++ )
		printf( "%lf\n", lambda[i] );

	return true;
}

int MaxEnt::predict( vector<string> feature, vector<pair<string,double> > &labelProbs )
{
	if ( !model->modelBuildDone() )
	{
		fprintf( stderr, "maxent model has not been build!\n");
		return -1;
	}
	int i, j;
	SpElem *x = new SpElem[feature.size()+1];
	int id;
	for( i = 0, j = 0; i < feature.size(); i ++ )
		if ( model->getFeaId(feature[i],id) )
		{
			x[j].index = id;
			x[j].value = 1;
			j ++;
		}
	x[j].index = -1; x[j].value = 0;

	double *probs = new double[classNumber];
	getProbs( x, probs );

	labelProbs.clear();
	int maxProbIndex = -1;
	for( i = 0; i < classNumber; i ++ )
	{
		if ( i == 0 || probs[i] > probs[maxProbIndex] )
			maxProbIndex = i;
		string clsName;
		if ( !model->getClsName(i,clsName) )
			fprintf( stderr, "index %d doesn't have a class name\n", i );
		labelProbs.push_back(pair<string,double>(clsName,probs[i]));
	}

	delete []x;
	delete []probs;

	return maxProbIndex;
}

bool MaxEnt::parseLine( FILE *fin, FILE *fout, bool withClassLabel )
{
	vector<string> feature;
	int c;
	int cnt = 0;
	enum State{ SPACE, WORD, NONE };
	int state = NONE;
	VarStr word;
	bool first = true;
	while( (c=fgetc(fin)) != EOF )
	{
		if ( c == '\n' )
			break;
		cnt ++;
		if ( state == NONE )
		{
			if ( isSpace(c) )
				state = SPACE;
			else
			{
				word.add(c);
				state = WORD;
			}
			continue;
		}
		if ( isSpace(c) )
		{
			if ( state == WORD )
			{
				if ( !first )
					feature.push_back(word.c_str());
				else
				{
					if ( !withClassLabel )
						feature.push_back(word.c_str());
					first = false;
				}
				state = SPACE;
				word.clear();
			}
		}
		else
		{
			if ( state == SPACE )
			{
				word.add(c);
				state = WORD;
			}
			else
				word.add(c);
		}
	}
	if ( c == EOF && cnt == 0 )
		return false;
	if ( word.length() > 0 )
	{
		if ( !first || (first&&!withClassLabel) )
			feature.push_back(word.c_str());
	}

	if ( feature.size() > 0 )
	{
		vector<pair<string,double> > labelProbs;
		int pcls = predict(feature,labelProbs);
		fprintf(fout,"%s",labelProbs[pcls].first.c_str());
		for( int i = 0; i < labelProbs.size(); i ++ )
			fprintf(fout, "\t%s\t%lf", labelProbs[i].first.c_str(), labelProbs[i].second);
		fprintf(fout,"\n");
	}
	return true;
}

int MaxEnt::predict( const char *testFile, const char *predictFile, bool withClassLabel )
{
	if ( !model->modelBuildDone() )
	{
		fprintf( stderr, "maxent model has not been build!\n");
		return -1;
	}

	FILE *fin = fopen( testFile, "r" );
	FILE *fout = fopen( predictFile, "w" );
	if ( fin == NULL )
	{
		fprintf( stderr, "open test file failed! %s\n", testFile );
		return -2;
	}
	if ( fout == NULL )
	{
		fprintf( stderr, "open predict file failed! %s\n", predictFile );
		return -3;
	}

	while( parseLine(fin,fout,withClassLabel) )
		;

	fclose(fin);
	fclose(fout);

	return 1;
}
