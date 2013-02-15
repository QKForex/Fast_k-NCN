#include "Parallel_kNCN.h"

Parallel_kNCN::Parallel_kNCN() : Classifier() {}

Parallel_kNCN::Parallel_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples)
	: Classifier(k, nrTrainSamples, nrTestSamples) {}

Parallel_kNCN::~Parallel_kNCN() {
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;
}

void Parallel_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	distances = new Distance*[nrTestSamples];
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distances[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
}

int Parallel_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample) {	  
	if (k == 1) {
		return find1NN(trainSet, nrTrainSamples, testSample).sampleLabel;
	} else {
		return assignLabel(findkNCN_parallel(const_cast<SampleSet&> (trainSet), testSample));
	}
}

//
//	Find k Nearest Centroid Neighbors using threads (parallel)
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest centroid neighbors
//
//TODO: parallel
// 
const Distance* Parallel_kNCN::findkNCN_parallel(SampleSet& trainSet, const Sample& testSample) {
	using std::swap;

	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));

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
