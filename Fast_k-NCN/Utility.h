#pragma once

#include <iostream>

#include "Config.h"

namespace Utility {

	enum ClassifierType {KNN, SEQ_KNCN, PAR_KNCN, RAND_KNCN, 
		LIMV1_KNCN, LIMV2_KNCN, CACHE_KNCN};

	//struct SampleData {
	//	int label;
	//	SampleDim sampleDims;
	//};

	//struct SampleSetData {
	//	int nrLabels;
	//	int nrDims;
	//	int nrSamples;

	//	SampleData samples;
	//};

	//void crossValidateSamples(SampleSet* train, SampleSet* test);
	
	//void standardizeSamples(SampleSet* train, SampleSet* test);

}