#pragma once

#include "Classifier.h"

class Parallel_kNCN: public Classifier {
public:
	Parallel_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Parallel_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);

private:
	Parallel_kNCN();

	void findkNCN_parallel(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNCN_parallel(SampleSet& trainSet, const Sample& testSample);
};