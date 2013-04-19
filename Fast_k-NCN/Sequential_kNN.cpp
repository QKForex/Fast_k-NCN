#include "Sequential_kNN.h"

Sequential_kNN::Sequential_kNN() : Classifier() {}

Sequential_kNN::Sequential_kNN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {
	distances = new Distance*[nrTestSamples];
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) {
		distances[distIndex] = new Distance[nrTrainSamples];
		std::fill(distances[distIndex], distances[distIndex]+nrTrainSamples, Distance(-1,-1, FLT_MAX));
	}
}

Sequential_kNN::~Sequential_kNN() {
	if (!results) { delete[] results; }
	if (!nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
	if (!distances) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
		delete[] distances;
	}
}

void Sequential_kNN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
void Sequential_kNN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		results[samIndex] = classifySample(trainSet, testSet[samIndex]);
	}
}

int Sequential_kNN::classifySample(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  
	if (k == 1) {
		return Classifier::find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNN(trainSet, testSample, testSampleDists, testSampleNNdists, k);
		return Classifier::assignLabel(testSampleNNdists, k);
	}
}

int Sequential_kNN::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, this->distances[testSample.index],
		this->nndists[testSample.index], this->k);
}

//
//	Wrapper for find1NN that uses distances attribute instead of specified distances
//
const Distance Sequential_kNN::find1NN(const SampleSet& trainSet, const Sample& testSample) {
	return Classifier::find1NN(trainSet, testSample, this->distances[testSample.index]);
}

//
//	Find k Nearest Neighbors 
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest neighbors
//
//	sorted list of k NNs (ascending, smallest first)
//	initialize with max float remaining NNs from dists list
//	if current neighbor is closer than the furthest one
//	in the list find a place for current NN
//	for every NN that is in the nndist list already
//	find suitable position in list (starting from the furthest)
//	if the distance is smaller than distance of the current NN
//	to be added, move the NN in list
//	move NNs to make place for current new NN
//	this should happen just once
//
void Sequential_kNN::findkNN(const SampleSet& trainSet, const Sample& testSample,
										Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	
	for (int distsIndex = 0; distsIndex < nrTrainSamples; distsIndex++) {
		if (testSampleDists[distsIndex].distValue < testSampleNNdists[k - 1].distValue) {
			for (int distsInNndistsIndex = 0; distsInNndistsIndex < k; distsInNndistsIndex++) {
				if (testSampleDists[distsIndex].distValue < testSampleNNdists[distsInNndistsIndex].distValue) {
					for (int distsToMoveIndex = k - 1; distsToMoveIndex > distsInNndistsIndex; distsToMoveIndex--) {
						testSampleNNdists[distsToMoveIndex] = testSampleNNdists[distsToMoveIndex - 1];
					}
					testSampleNNdists[distsInNndistsIndex] = testSampleDists[distsIndex];
					break;
				}
			}
		}
	}
}

//
//	Wrapper for findkNN that uses distances attribute instead of specified distances
//
void Sequential_kNN::findkNN(const SampleSet& trainSet, const Sample& testSample) {
	return findkNN(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}

int Sequential_kNN::assignLabel(const int testSampleIndex) {
	return Classifier::assignLabel(this->distances[testSampleIndex], this->k);
}