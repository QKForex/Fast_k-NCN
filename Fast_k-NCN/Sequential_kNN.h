#pragma once

#include "Classifier.h"

class Sequential_kNN: public Classifier {
public:
	static LoggerPtr logger;

	Distance** distances; // distances for all combinations of train and test samples, preprocessed

	Sequential_kNN(const int k, const int nrTrainSamples, const int nrTestSamples);
	~Sequential_kNN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);

private:
	Sequential_kNN();

	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample);

	void findkNN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	void findkNN(const SampleSet& trainSet, const Sample& testSample);

	int assignLabel(const int testSampleIndex);

};