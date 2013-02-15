#include "Sequential_kNN.h"

Sequential_kNN::Sequential_kNN() : Classifier() {}

Sequential_kNN::Sequential_kNN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {
	results = new int[nrTestSamples];
	distances = new Distance*[nrTestSamples];
}

Sequential_kNN::~Sequential_kNN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
	delete[] results;
}

//
//	Find optimal k - number of nearest nieghbors using leave-one-out method
//	
//	Input:
//	Output:
//
//const int Sequential_kNN::learnOptimalK(const SampleSet& trainSet, const int largestK) {
//	int optimalK = 1;
//	for (int k = 1; k < largestK; k++) {
//		for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
//
//
//			optimalK = k;
//		}
//	}
//
//}

void Sequential_kNN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}

int Sequential_kNN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		return assignLabel(findkNN(trainSet, testSample, testSampleDists));
	}
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
//TODO: extend interface by train and test samples
const Distance* Sequential_kNN::findkNN(const SampleSet& trainSet, const Sample& testSample,
										Distance* testSampleDists) {	
	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	if (nndists != NULL) {
		fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));
	} else {
		throw bad_alloc();
	}

	for (int distsIndex = 0; distsIndex < nrTrainSamples; distsIndex++) {
		if (testSampleDists[distsIndex].distValue < nndists[k - 1].distValue) {
			for (int distsInNndistsIndex = 0; distsInNndistsIndex < k; distsInNndistsIndex++) {
				if (testSampleDists[distsIndex].distValue < nndists[distsInNndistsIndex].distValue) {
					for (int distsToMoveIndex = k - 1; distsToMoveIndex > distsInNndistsIndex; distsToMoveIndex--) {
						nndists[distsToMoveIndex] = nndists[distsToMoveIndex - 1];
					}
					nndists[distsInNndistsIndex] = testSampleDists[distsIndex];
					break;
				}
			}
		}
	}
	return nndists;
}

//
//	Wrapper for findkNN that uses distances attribute instead of specified distances
//
const Distance* Sequential_kNN::findkNN(const SampleSet& trainSet, const Sample& testSample) {
	return findkNN(trainSet, testSample, distances[testSample.index]);
}