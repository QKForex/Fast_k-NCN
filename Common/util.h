#pragma once
#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <iostream>
#include "SampleSet.h"
#include "config.h"

using namespace std;

namespace Common {

	static int remainder_table[] = { 0, 3, 2, 1 };

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

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line);

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line);

	void copySampleDims(SampleDim* src, int nrDims, SampleDim* dst);

	void standardize(SampleSet* train, SampleSet* test);
};

#endif /* COMMON_UTIL_H_ */