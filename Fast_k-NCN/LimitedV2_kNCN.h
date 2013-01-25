#pragma once

#include "Classifier.h"

class LimitedV2_kNCN: public Classifier {
public:
	LimitedV2_kNCN();
	LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~LimitedV2_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);

};