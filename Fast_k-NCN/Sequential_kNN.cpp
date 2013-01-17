#include "Sequential_kNN.h"

Sequential_kNN::Sequential_kNN()
	: Classifier() {}

Sequential_kNN::Sequential_kNN(SampleSet* train, SampleSet* test, int k)
	: Classifier(train, test, k) {}

Sequential_kNN::~Sequential_kNN() {
	trainSet = NULL;
	testSet = NULL; // Classifier should not free memory for sets
}

Distance** Sequential_kNN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	const int nrTrainSamples = trainSet.getNrSamples();
	const int nrTestSamples = testSet.getNrSamples();

	Distance** distancesToTestSamples = new Distance*[nrTestSamples];
	
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distancesToTestSamples[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
	
	return distancesToTestSamples;
}

int* Sequential_kNN::classify(const SampleSet& trainSet, const SampleSet& testSet, const int k, Distance** dists) {
	const int nrTrainSamples = trainSet.getNrSamples();
	const int nrTestSamples = testSet.getNrSamples();		  
	int* results = new int[nrTestSamples];

	if (k == 1) {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = find1NN(dists[samIndex], nrTrainSamples).sampleLabel;
		}
	} else {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = assignLabel(findkNN(dists[samIndex], nrTrainSamples, k), k);
		}
	}
	return results;
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
//TODO extend interface by train and test samples
const Distance* Sequential_kNN::findkNN(Distance* dists, int distsSize, int k) {
	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));

	for (int distsIndex = 0; distsIndex < distsSize; distsIndex++) {
		if (dists[distsIndex].distValue < nndists[k - 1].distValue) {
			for (int distsInNndistsIndex = 0; distsInNndistsIndex < k; distsInNndistsIndex++) {
				if (dists[distsIndex].distValue < nndists[distsInNndistsIndex].distValue) {
					for (int distsToMoveIndex = k - 1; distsToMoveIndex > distsInNndistsIndex; distsToMoveIndex--) {
						nndists[distsToMoveIndex] = nndists[distsToMoveIndex - 1];
					}
					nndists[distsInNndistsIndex] = dists[distsIndex];
					break;
				}
			}
		}
	}
	return nndists;
}
