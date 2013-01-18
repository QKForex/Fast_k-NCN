#pragma once

#include "Classifier.h"

class Sequential_kNCN: public Classifier {
public:
	Sequential_kNCN();
	Sequential_kNCN(const int k);
	~Sequential_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet);

private:
	const Distance* findkNCN(const SampleSet& trainSet, const int nrTrainSamples,
		const Sample& testSample);

};
