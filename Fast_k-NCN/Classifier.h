#pragma once

#include <vector>
#include <algorithm>
#include <climits>

#include <time.h>
#include <windows.h>

#include <boost\bind.hpp>

#include "DistanceCalculation.h"
#include "SampleSet.h"

using namespace Common;

//
//	Abstract base class for Classifiers
//
class Classifier { //TODO: singleton
public:
	Distance** distances; // all distances for all combinations of train and test samples
	Distance** nndists; // distances to k nearest neighbors for all test samples
	int* results; // results of nrClasses

	int k;
	int nrTrainSamples;
	int nrTestSamples;
	int nrClassificationErrors;
	float errorRate;

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

	//TODO: find1NN Parallel
	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);
	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample);
	
	int assignLabel(const int testSampleIndex);
	int calculateError(const SampleSet& orig, const int* results);
};


