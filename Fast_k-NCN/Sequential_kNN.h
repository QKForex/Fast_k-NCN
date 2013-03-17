#pragma once

#include "Classifier.h"

class Sequential_kNN: public Classifier {
public:
	Sequential_kNN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);

private:
	Sequential_kNN();

	void findkNN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNN(const SampleSet& trainSet, const Sample& testSample);

};