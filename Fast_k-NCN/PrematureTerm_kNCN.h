#pragma once

#include "Classifier.h"

class PrematureTerm_kNCN: public Classifier {
public:
	static LoggerPtr logger;
	SampleSet centroids;

	PrematureTerm_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples,
		const int nrClasses, const int nrDims, const int threshold);
	~PrematureTerm_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);

private:
	int threshold;

	PrematureTerm_kNCN();

	const Distance find1NNPrematureTerm(const SampleSet& trainSet, const Sample& testSample);

	void findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleNNdists, const int k);
	void findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample);

	int assignLabel(const int testSampleIndex);
};

