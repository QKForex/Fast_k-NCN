#pragma once

#include "Classifier.h"

class Sequential_kNCN: public Classifier {
public:
	Sequential_kNCN();
	Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN(const SampleSet& trainSet, const Sample& testSample);
	void swapSamples(SampleSet& trainSet, const int samIndexToMoveToBack, const int samIndexToMoveFromBack);

};
