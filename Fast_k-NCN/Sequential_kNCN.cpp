#include "Sequential_kNCN.h"

Sequential_kNCN::Sequential_kNCN() : Classifier() {}

Sequential_kNCN::Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

Sequential_kNCN::~Sequential_kNCN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
}

void Sequential_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}


//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
int* Sequential_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	int* results = new int[nrTestSamples];

	if (k == 1) {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = find1NN(trainSet, nrTrainSamples, testSet[samIndex]).sampleLabel;
		}
	} else {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = assignLabel(findkNCN(const_cast<SampleSet&> (trainSet), testSet[samIndex]));
		}
	}
	return results;
}

//
//	Find k Nearest Centroid Neighbors 
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest centroid neighbors
//
//	sorted list of k NNs (ascending, smallest first)
//	initialize with max float
//	first kNCN (is equal to kNN)
//	instead of comparing current kNCN candidate to already chosen kNCNs
//	distsSize times in loop, 
//	we set the distance to FLT_MAX for chosen kNCN sample in distances once
//	this excludes first kNCN from being chosen again
//	this action will be repeated for the rest 
//	distances[nndists[0].sampleIndex].distValue = FLT_MAX;
//	check if given sample is not kNCN already
//
//TODO: inne rozw. ew. mozna zamienic z ostatnim i iterowaæ po n - j
// 
//	trainSet needs to be changed, moving already used to the back of array, swap samples
//
const Distance* Sequential_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	using std::swap;
	Distance* nndists = new Distance[k];
	fill(nndists, nndists+k, Distance(-1, -1, FLT_MAX));

	SampleSet centroids(trainSet.nrClasses, trainSet.nrDims, k);
	
	nndists[0] = find1NN(trainSet, trainSet.nrSamples, testSample);
	centroids[0] = trainSet[nndists[0].sampleIndex];
	//trainSet[nndists[0].sampleIndex].swap(trainSet[trainSet.nrSamples - 1]);
	trainSet.swapSamples(nndists[0].sampleIndex, trainSet.nrSamples - 1);

	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		Sample currentCentroid(-1, -1, trainSet.nrDims);
		for (int samIndex = 0; samIndex < trainSet.nrSamples - centroidIndex; samIndex++) {
			for (int dimIndex = 0; dimIndex < trainSet.nrDims; dimIndex++) {
				currentCentroid[dimIndex] =
					(centroids[centroidIndex-1][dimIndex] * ((float)centroidIndex/(centroidIndex+1)))
					+ (trainSet[samIndex][dimIndex] / (centroidIndex+1));
				//currentCentroid[dimIndex] =	centroids[centroidIndex-1][dimIndex] + trainSet[samIndex][dimIndex];
			}
			Distance currentNNdist(trainSet[samIndex].index, trainSet[samIndex].label,
				countManhattanDistance(currentCentroid, testSample, trainSet.nrDims)); //TODO: hardcoded Manhattan
			if (currentNNdist.distValue < nndists[centroidIndex].distValue) {
				currentCentroid.index = trainSet[samIndex].index; // do not need to do that
				currentCentroid.label = trainSet[samIndex].label;
				nndists[centroidIndex] = currentNNdist;
				centroids[centroidIndex] = currentCentroid;
			}
		}
		//trainSet[nndists[centroidIndex].sampleIndex].swap(trainSet[trainSet.nrSamples-1 - centroidIndex]);
		trainSet.swapSamples(nndists[centroidIndex].sampleIndex, trainSet.nrSamples-1 - centroidIndex);
	}

	return nndists;
}

inline void Sequential_kNCN::swapSamples(SampleSet& trainSet, const int samIndexToMoveToBack, const int samIndexToMoveFromBack) {
	//trainSet[samIndexToMoveToBack].swap(trainSet[samIndexToMoveFromBack]);
	Sample tempSample(trainSet[samIndexToMoveToBack]);
	trainSet[samIndexToMoveToBack] = trainSet[samIndexToMoveFromBack];
	trainSet[samIndexToMoveFromBack] = tempSample;
}
