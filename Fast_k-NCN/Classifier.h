#pragma once

#include <algorithm>
#include <climits>

#include <time.h>
#include <windows.h>

#include "Distance.h"
#include "DistanceCalculation.h"
#include "SampleSet.h"

using namespace std;
using namespace Common;

//
//	Abstract base class for Classifiers
//
class Classifier { //TODO singleton
public:
	int k;
	Distance** distances;
	int nrTrainSamples;
	int nrTestSamples;

	virtual ~Classifier() {}; // cannot implement pure virtual destructor
	virtual void preprocess(const SampleSet& trainSet, const SampleSet& testSet) = 0;
	virtual int* classify(const SampleSet& trainSet, const SampleSet& testSet) = 0;

	int countError(int* const& result, const SampleSet& orig);

protected:
	Classifier();
	Classifier(const int k);

	const Distance find1NN(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);
	int assignLabel(const Distance* dists);

};


