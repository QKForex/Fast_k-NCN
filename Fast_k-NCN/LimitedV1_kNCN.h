#pragma once

#include "Classifier.h"

class LimitedV1_kNCN: public Classifier {
public:
	LimitedV1_kNCN();
	LimitedV1_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~LimitedV1_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);

};