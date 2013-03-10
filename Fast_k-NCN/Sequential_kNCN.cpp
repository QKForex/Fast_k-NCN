#include "Sequential_kNCN.h"

Sequential_kNCN::Sequential_kNCN() : Classifier() {}

Sequential_kNCN::Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {
	//TODO: initialize centroids**
}

Sequential_kNCN::~Sequential_kNCN() {
	delete[] results;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
	delete[] nndists;
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
	delete[] distances;
}

void Sequential_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], this->distances[samIndex]);
	}
}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
int Sequential_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
								   Distance* testSampleDists) {	  
	if (k == 1) {
		return find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(const_cast<SampleSet&> (trainSet), testSample, testSampleDists);
		return assignLabel(testSample.index);
	}
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
void Sequential_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample,
										  Distance* testSampleDists) {
	SampleSet centroids(trainSet.nrClasses, trainSet.nrDims, k); //TODO: move to attributes for Seq_kNCN
	
	nndists[testSample.index][0] = find1NN(trainSet, testSample, testSampleDists);
	centroids[0] = trainSet[nndists[testSample.index][0].sampleIndex];
	//trainSet[nndists[testSample.index][0].sampleIndex].swap(trainSet[trainSet.nrSamples - 1]);
	trainSet.swapSamples(nndists[testSample.index][0].sampleIndex, trainSet.nrSamples - 1);

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
			if (currentNNdist.distValue < nndists[testSample.index][centroidIndex].distValue) {
				currentCentroid.index = trainSet[samIndex].index; // do not need to do that
				currentCentroid.label = trainSet[samIndex].label;
				nndists[testSample.index][centroidIndex] = currentNNdist;
				centroids[centroidIndex] = currentCentroid;
			}
		}
		//trainSet[nndists[testSample.index][centroidIndex].sampleIndex].swap(trainSet[trainSet.nrSamples-1 - centroidIndex]);
		trainSet.swapSamples(nndists[testSample.index][centroidIndex].sampleIndex, trainSet.nrSamples-1 - centroidIndex);
	}
}

void Sequential_kNCN::findkNCN(SampleSet& trainSet, const Sample& testSample) {
	return findkNCN(trainSet, testSample, this->distances[testSample.index]);
}

