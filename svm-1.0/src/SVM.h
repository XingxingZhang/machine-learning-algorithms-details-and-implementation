#ifndef __SVM_H__
#define __SVM_H__

#include "SVM_Solver.h"

class SVM
{
public:
	static void train( const SVM_Input &input, SVM_Model &outModel );
	static double predict( const SVM_Model &model, const SpElem *x );
	/**
	 * load svm model from a file. Notice that you must delete xspace
	 * and the end of the program
	 */
	static bool load( const char* infile, SVM_Model &outModel, SpElem *&xspace );
	/**
	 * save svm model to a file.
	 */
	static bool save( const char *outfile, const SVM_Model &model );
};

#endif
