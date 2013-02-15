#pragma once

#include "Classifier.h"

class CacheEfficient_kNCN: public Classifier {
public:
	CacheEfficient_kNCN();
	CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~CacheEfficient_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);

private:
	const Distance* CacheEfficient_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample);

};