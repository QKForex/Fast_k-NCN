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
class Classifier { //TODO: singleton
public:
	int k;
	Distance** distances; // distances between all samples
	int nrTrainSamples;
	int nrTestSamples;
	int nrClassificationErrors;
	float errorRate;

	int* results;

	Distance* nndists; // distances to k nearest neighbors

	virtual ~Classifier() {}; // cannot implement pure virtual destructor
	virtual void preprocess(const SampleSet& trainSet, const SampleSet& testSet) = 0;
	virtual int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* dists) = 0;
	
	const int learnOptimalK(const SampleSet& trainSet, const int largestK);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	void calculateErrorRate(const SampleSet& orig);

protected:
	Classifier();
	Classifier(const int k, const int nrTrainSamples, const int nrTestSamples);

	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample,
		Distance** dists);
	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample);
	
	int assignLabel(const Distance* dists);

	// wrappers
	int calculateError(const int* results, const SampleSet& orig);
	int classifySample(const SampleSet& trainSet, const SampleSet& testSet);
};


