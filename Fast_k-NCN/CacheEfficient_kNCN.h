#pragma once

#include "Classifier.h"

class CacheEfficient_kNCN: public Classifier {
public:
	CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~CacheEfficient_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);

private:
	CacheEfficient_kNCN();

	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);

};