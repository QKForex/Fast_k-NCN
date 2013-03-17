#pragma once

#include "Classifier.h"

class Sequential_kNCN: public Classifier {
public:
	// centroids
	
	Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
private:
	Sequential_kNCN();

	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);
};
