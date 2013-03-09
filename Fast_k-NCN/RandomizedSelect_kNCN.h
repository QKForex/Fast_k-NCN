#pragma once

#include "Classifier.h"

class RandomizedSelect_kNCN: public Classifier {
public:
	RandomizedSelect_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~RandomizedSelect_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);

private:
	RandomizedSelect_kNCN();
	
	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);
};