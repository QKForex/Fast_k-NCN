#include "LimitedV2_kNCN.h"

LimitedV2_kNCN::LimitedV2_kNCN() : Classifier() {}

LimitedV2_kNCN::LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

LimitedV2_kNCN::~LimitedV2_kNCN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
}

void LimitedV2_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}

int* LimitedV2_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
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

const Distance* LimitedV2_kNCN::findkNCN(const SampleSet& trainSet, const int nrTrainSamples, const Sample& testSample) {
	Distance* nndists = NULL;
	
	return nndists;
}