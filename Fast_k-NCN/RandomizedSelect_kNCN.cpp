#include "RandomizedSelect_kNCN.h"

RandomizedSelect_kNCN::RandomizedSelect_kNCN() : Classifier() {}

RandomizedSelect_kNCN::RandomizedSelect_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

RandomizedSelect_kNCN::~RandomizedSelect_kNCN() {
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

void RandomizedSelect_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

int RandomizedSelect_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists, testSampleNNdists, k);
		return assignLabel(testSampleNNdists, k);
	}
}

void RandomizedSelect_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample,
												Distance* testSampleDists, Distance* testSampleNNdists, const int k) {

}

void RandomizedSelect_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	findkNCN(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}