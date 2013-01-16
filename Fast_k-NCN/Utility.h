#pragma once

#include <iostream>

#include "Config.h"
#include "SampleSet.h"

using namespace std;
using namespace Common;

namespace Utility {

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

	void copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst);

	//void crossValidateSamples(SampleSet* train, SampleSet* test);
	
	//void standardizeSamples(SampleSet* train, SampleSet* test);

}