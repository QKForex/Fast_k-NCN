#include "CacheEfficient_kNCN.h"

CacheEfficient_kNCN::CacheEfficient_kNCN() : Classifier() {}

CacheEfficient_kNCN::CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {
	distances = new Distance*[nrTestSamples];
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) {
		distances[distIndex] = new Distance[nrTrainSamples];
		std::fill(distances[distIndex], distances[distIndex]+nrTrainSamples, Distance(-1,-1, FLT_MAX));
	}
}

CacheEfficient_kNCN::~CacheEfficient_kNCN() {
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

void CacheEfficient_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

void CacheEfficient_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		results[samIndex] = classifySample(trainSet, testSet[samIndex],
			distances[samIndex], nndists[samIndex], k);
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