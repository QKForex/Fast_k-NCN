#include "Classifier.h"

Classifier::Classifier() : k(1), nrTrainSamples(0), nrTestSamples(0) {
	distances = NULL;
}

Classifier::Classifier(const int k, const int nrTrainSamples, const int nrTestSamples)
	: k(k), nrTrainSamples(nrTrainSamples), nrTestSamples(nrTestSamples) {
	distances = NULL;
}

//
//	Find first Nearest Neighbor - simplified version 
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest neighbors
//
const Distance Classifier::find1NN(const SampleSet& trainSet, const int nrTrainSamples,
	const Sample& testSample) {							   
	Distance nearestNeighbourDist = distances[testSample.index][0];
	//TODO to parallelize somehow, e.g. compare in 4 threads,
	// and then compare 4 results in the end
	for (int distsIndex = 1; distsIndex < nrTrainSamples; distsIndex++) {
		if (distances[testSample.index][distsIndex].distValue < nearestNeighbourDist.distValue) {
			nearestNeighbourDist = distances[testSample.index][distsIndex];
		}
	}
	return nearestNeighbourDist;
}

//	
//	Assigns sample to most frequent label
//	from k nearest neighbors found by findkNN function
//
//	Input:
//	Output:
// 
int Classifier::assignLabel(const Distance* dists) {
	if (k == 1) { return dists[0].sampleLabel; }

	//  first find largest class label
	// 	create array of distsSize of the greatest class label so that we can increase
	//	the value under that index every time that label is met
	int i = 0;
	int largestLabel = dists[i].sampleLabel;
	for (i = 1; i < k; i++) {
		if (largestLabel < dists[i].sampleLabel) {
			largestLabel = dists[i].sampleLabel;
		}
	}
	
	int* freqs = new int[largestLabel + 1];
	for (i = 0; i < largestLabel + 1; i++) { freqs[i] = 0; }

	int result = 0;
	for (i = 0; i < k; i++)	{
		// in case of draw choose the label with smallest distance,
		// change ">" to ">=" to choose the last, largest
		if (++freqs[dists[i].sampleLabel] > freqs[result]) {
			result = dists[i].sampleLabel;
		}
	}

	delete[] freqs;

	return result;
}
