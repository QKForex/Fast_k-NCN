#include "LimitedV2_kNCN.h"

LimitedV2_kNCN::LimitedV2_kNCN() : Classifier() {}

LimitedV2_kNCN::LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

LimitedV2_kNCN::~LimitedV2_kNCN() {
	if (!results) { delete[] results; }
	if (!nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
	if (!distances) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
		delete[] distances;
	}
}

void LimitedV2_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

int LimitedV2_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists, testSampleNNdists, k);
		return assignLabel(testSampleNNdists, k);
	}
}

void LimitedV2_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample,
										 Distance* testSampleDists, Distance* testSampleNNdists, const int k) {

}

void LimitedV2_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	findkNCN(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}