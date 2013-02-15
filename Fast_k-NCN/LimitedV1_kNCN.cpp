#include "LimitedV1_kNCN.h"

LimitedV1_kNCN::LimitedV1_kNCN() : Classifier() {}

LimitedV1_kNCN::LimitedV1_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

LimitedV1_kNCN::~LimitedV1_kNCN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
}

void LimitedV1_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}

int LimitedV1_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample).sampleLabel;
	} else {
		return assignLabel(findkNCN(const_cast<SampleSet&> (trainSet), testSample));
	}
}

const Distance* LimitedV1_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	Distance* nndists = NULL;
	
	return nndists;
}