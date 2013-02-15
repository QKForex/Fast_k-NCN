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

int CacheEfficient_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		return assignLabel(findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists));
	}
}

const Distance* CacheEfficient_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample, 
						 Distance* testSampleDists) {
	Distance* nndists = NULL;
	
	return nndists;
}

const Distance* CacheEfficient_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	Distance* nndists = NULL;
	
	return nndists;
}