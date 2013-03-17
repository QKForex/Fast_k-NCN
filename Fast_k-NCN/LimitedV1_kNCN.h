#pragma once

#include "Classifier.h"

class LimitedV1_kNCN: public Classifier {
public:
	LimitedV1_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~LimitedV1_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);

private:
	LimitedV1_kNCN();

	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);
};