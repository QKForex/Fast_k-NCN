#pragma once

#include "Classifier.h"

class LimitedV2_kNCN: public Classifier {
public:
	Distance** distances; // distances for all combinations of train and test samples, preprocessed

	LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~LimitedV2_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);

private:
	LimitedV2_kNCN();

	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);
};