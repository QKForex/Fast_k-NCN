#include "CacheEfficient_kNCN.h"

CacheEfficient_kNCN::CacheEfficient_kNCN() : Classifier() {}

CacheEfficient_kNCN::CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

CacheEfficient_kNCN::~CacheEfficient_kNCN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
}

void CacheEfficient_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}

int* CacheEfficient_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	int* results = new int[nrTestSamples];

	if (k == 1) {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = find1NN(trainSet, nrTrainSamples, testSet[samIndex]).sampleLabel;
		}
	} else {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = assignLabel(findkNCN(trainSet, nrTrainSamples, testSet[samIndex]));
		}
	}
	return results;
}

const Distance* CacheEfficient_kNCN::findkNCN(const SampleSet& trainSet, const int nrTrainSamples, const Sample& testSample) {
	Distance* nndists = NULL;
	
	return nndists;
}