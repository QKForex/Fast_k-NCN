#pragma once

#include "Classifier.h"

class Parallel_kNCN: public Classifier {
public:
	Parallel_kNCN();
	Parallel_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Parallel_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN_parallel(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);

};