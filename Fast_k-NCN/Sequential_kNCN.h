#pragma once

#include "Classifier.h"

class Sequential_kNCN: public Classifier {
public:
	Distance** distances; // distances for all combinations of train and test samples, preprocessed
	// centroids
	
	Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);

private:
	Sequential_kNCN();

	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample);

	void findkNCN(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNCN(SampleSet& trainSet, const Sample& testSample);

	int assignLabel(const int testSampleIndex);
};
