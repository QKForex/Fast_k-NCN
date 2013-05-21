#pragma once

#include "Classifier.h"

class CacheEfficient_kNCN: public Classifier {
public:
	static LoggerPtr logger;

	SampleSet centroids;

	Distance**** blockDistances;
	NCNDistance**** blockNCNDists;

	NCNDistance** closestNCNDists;
	
	int nrSamplesInTrainBlock;
	int nrSamplesInTestBlock;
	int nrTrainSetBlocks;
	int nrTestSetBlocks;
	
	int firstRemainingTrainSampleIndex;
	int nrRemainingTrainSamples;
	int firstRemainingTestSampleIndex;
	int nrRemainingTestSamples;

	CacheEfficient_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples, 
		const int nrClasses, const int nrDims, const int nrSamplesInBlock);
	~CacheEfficient_kNCN();

	void preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	void classify(const SampleSet& trainSet, const SampleSet& testSet);
	int classifySample(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, Distance* testSampleNNdists, const int k);

private:
	CacheEfficient_kNCN();

	void classifyBlock(const SampleSet& trainSetBlock, const SampleSet& testSetBlock,
		Distance** distances, NCNDistance** ncndists, const int trainBlockOffset, const int testBlockOffset);

	void findkNCN(const SampleSet& trainSet, const Sample& testSample,
		Distance* testSampleDists, NCNDistance* testSampleNCNdists, const int k, const int trainBlockOffset, const int testBlockOffset);

	int assignLabel(const NCNDistance* testSampleNCNdists, const int k);
};
