#pragma once

#include "Classifier.h"

class Sequential_kNN: public Classifier {
public:
	Sequential_kNN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNN();

	//const int learnOptimalK(const SampleSet& trainSet, const int largestK);
	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);

private:
	Sequential_kNN();

	void findkNN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists);
	void findkNN(const SampleSet& trainSet, const Sample& testSample);

};