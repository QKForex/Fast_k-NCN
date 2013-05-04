#pragma once

#include "Classifier.h"

class PrematureTerm_kNCN: public Classifier {
public:
	static LoggerPtr logger;

	//Distance** distances; // distances for all combinations of train and test samples, preprocessed
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

	void countDistancePrematureTerm(const Sample& trainSample, const Sample& testSample,
		Distance* distances, int samIndex, int threshold);
	Distance countDistancePrematureTerm(const Sample& trainSample, const Sample& testSample, int threshold);

	DistanceValue countManhattanDistancePrematureTerm(const Sample& train, const Sample& test,
		const int firstDim, const int lastDim);


	const Distance find1NNPrematureTerm(const SampleSet& trainSet, const Sample& testSample);

	void findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleNNdists, const int k);
	void findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample);

	int assignLabel(const int testSampleIndex);
};

