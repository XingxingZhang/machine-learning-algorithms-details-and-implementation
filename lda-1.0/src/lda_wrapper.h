#ifndef __LDA_WRAPPER_H__
#define __LDA_WRAPPER_H__

#include "lda.h"
#include "strid.h"

class LDAWrapper
{
public:
	LDAWrapper( const Para* _para );
	void readDocs( const char *trfile );
	void estimatePara( const char *modelfile );
	~LDAWrapper();
private:
	StrId wordID;
	int **doc;
	int *ndoc;			// ndoc[i] is the number of words in doc[i]
	int M;				// document number
	const Para *para;

	void saveModel( const char *modelfile, double **theta, double **phi );
};

#endif
