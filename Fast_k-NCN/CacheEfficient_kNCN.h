#pragma once

#include "Classifier.h"

class CacheEfficient_kNCN: public Classifier {
public:
	CacheEfficient_kNCN();
	CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~CacheEfficient_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);

};