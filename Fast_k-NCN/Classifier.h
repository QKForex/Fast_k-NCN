#pragma once

#include <vector>
#include <algorithm>
#include <climits>

#include <time.h>
#include <windows.h>

#include <boost\bind.hpp>

#include "Logger.h"
#include "DistanceCalculation.h"
#include "SampleSet.h"

using namespace Common;

//
//	Abstract base class for Classifiers
//
class Classifier {
public:
	Distance** nndists; // distances to k nearest neighbors for all test samples
	int* results; // labels assigned to every sample as number of one of classes, ordered by index

	int k;
	int nrTrainSamples;
	int nrTestSamples;
	int nrClassificationErrors;
	float errorRate;

	// moved here to print in results
	// LimitedV1_kNCN
	int maximalRobustRank;

	// LimitedV2_kNCN
	int* maximalRobustRanks;

	virtual ~Classifier() {}; // cannot implement pure virtual destructor
	virtual void preprocess(const SampleSet& trainSet, const SampleSet& testSet) = 0;
	virtual void classify(const SampleSet& trainSet, const SampleSet& testSet) = 0;
	virtual int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k) = 0;
	
	const int learnOptimalK(const SampleSet& trainSet, const int largestK);
	void calculateErrorRate(const SampleSet& orig);

protected:
	Classifier();
	Classifier(const int k, const int nrTrainSamples, const int nrTestSamples);

	//TODO: find1NN Parallel
	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample,
		const Distance* testSampleDists);
	
	int assignLabel(const Distance* testSampleNNdists, const int k);
	int calculateError(const SampleSet& orig, const int* results);
};
