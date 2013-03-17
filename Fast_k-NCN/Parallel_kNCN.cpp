#include "Parallel_kNCN.h"

Parallel_kNCN::Parallel_kNCN() : Classifier() {}

Parallel_kNCN::Parallel_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

Parallel_kNCN::~Parallel_kNCN() {
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

void Parallel_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

int Parallel_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN_parallel(const_cast<SampleSet&> (trainSet), testSample, testSampleDists, testSampleNNdists, k);
		return assignLabel(testSampleNNdists, k);
	}
}

//
//	Find k Nearest Centroid Neighbors using threads (parallel)
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest centroid neighbors
//
//TODO: parallel
// 
void Parallel_kNCN::findkNCN_parallel(SampleSet& trainSet, const Sample& testSample,
										 Distance* testSampleDists, Distance* testSampleNNdists, const int k) {

}

void Parallel_kNCN::findkNCN_parallel(SampleSet& trainSet, const Sample& testSample) {
	findkNCN_parallel(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}
