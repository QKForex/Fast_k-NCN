#include "CacheEfficient_kNCN.h"

CacheEfficient_kNCN::CacheEfficient_kNCN() : Classifier() {}

CacheEfficient_kNCN::CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

CacheEfficient_kNCN::~CacheEfficient_kNCN() {
	delete[] results;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
	delete[] nndists;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
	delete[] distances;
}

void CacheEfficient_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

int CacheEfficient_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists, testSampleNNdists, k);
		return assignLabel(testSampleNNdists, k);
	}
}

void CacheEfficient_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample, 
						 Distance* testSampleDists, Distance* testSampleNNdists, const int k) {

}

void CacheEfficient_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	findkNCN(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}