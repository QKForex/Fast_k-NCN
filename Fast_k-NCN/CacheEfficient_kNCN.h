#pragma once

#include "Classifier.h"

class CacheEfficient_kNCN: public Classifier {
public:
	static LoggerPtr logger;

	SampleSet centroids;
	
	int nrSamplesInTrainBlock;
	int nrSamplesInTestBlock;
	int nrTrainSetBlocks;
	int nrTestSetBlocks;
	
	CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples, 
		const int nrClasses, const int nrDims, const int nrSamplesInBlock);
	~CacheEfficient_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);
	int classifySample(const SampleSet& trainSet, const Sample& testSample);

private:
	CacheEfficient_kNCN();

	Distance** classifyBlock(const SampleSet& trainSetBlock,
		const SampleSet& testSetBlock, const int testBlockOffset);

	const Distance find1NN(const SampleSet& trainSet, const Sample& testSample);

	void findkNCN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k, const int testBlockOffset);
};
