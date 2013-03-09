#include "LimitedV2_kNCN.h"

LimitedV2_kNCN::LimitedV2_kNCN() : Classifier() {}

LimitedV2_kNCN::LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

LimitedV2_kNCN::~LimitedV2_kNCN() {
	delete[] results;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
	delete[] nndists;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
	delete[] distances;
}

void LimitedV2_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

int LimitedV2_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists);
		return assignLabel(testSample.index);
	}
}

void LimitedV2_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample,
										 Distance* testSampleDists) {

}

void LimitedV2_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	findkNCN(trainSet, testSample, distances[testSample.index]);
}