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

int* Parallel_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	int* results = new int[nrTestSamples];

	if (k == 1) {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = find1NN(trainSet, nrTrainSamples, testSet[samIndex]).sampleLabel;
		}
	} else {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = assignLabel(findkNCN_parallel(trainSet, nrTrainSamples, testSet[samIndex]));
		}
	}
	return results;
}

//
//	Find k Nearest Centroid Neighbors using threads (parallel)
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest centroid neighbors
//
//TODO: parallel
// 
const Distance* Parallel_kNCN::findkNCN_parallel(const SampleSet& trainSet, const int nrTrainSamples, const Sample& testSample) {
	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));

	//SampleSet centroids = (Sample*) malloc(k * sizeof(Sample));
	
	DistanceValue* centroids = (DistanceValue*) malloc(k * sizeof(DistanceValue));
	nndists[0] = find1NN(trainSet, nrTrainSamples, testSample);
	centroids[0] = nndists[0].distValue;
	DistanceValue* tempCentroids = (DistanceValue*) malloc(nrTrainSamples * sizeof(DistanceValue));
	DistanceValue* tempCentroidsDistances = (DistanceValue*) malloc(nrTrainSamples * sizeof(DistanceValue));
	DistanceValue sum = 0;
	Distance minCentroidDistance = Distance(-1,-1,FLT_MAX);
	bool iskNCN = false;
	int sampleDims = 30; //TODO: hardcoded
	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		sum = 0;
		//minCentroidDistance = Sample(-1, sampleDims, ;
		for (int previousCentroidIndex = 0; previousCentroidIndex < centroidIndex; previousCentroidIndex++) {
			//sum += centroids[previousCentroidIndex];
		}
		for (int distsIndex = 0; distsIndex < nrTrainSamples; distsIndex++) {
			iskNCN = false;
			for (int i = 0; i < centroidIndex; i++) {
				if (nndists[i].sampleIndex == distsIndex) {
					iskNCN = true;
					break;
				}
			}

			if (!iskNCN) {			
				tempCentroids[distsIndex] = (distances[testSample.index][distsIndex].distValue + sum)/centroidIndex;
				tempCentroidsDistances[distsIndex] =
					(distances[testSample.index][distsIndex].distValue - tempCentroids[distsIndex]) * 
					(distances[testSample.index][distsIndex].distValue - tempCentroids[distsIndex]);
				if (minCentroidDistance.distValue > tempCentroidsDistances[distsIndex]) {
					minCentroidDistance = distances[testSample.index][distsIndex];
					minCentroidDistance.distValue = tempCentroidsDistances[distsIndex];
				}
			}

		}

		nndists[centroidIndex] = distances[testSample.index][minCentroidDistance.sampleIndex];

		//distances[nndists[centroidIndex].sampleIndex].distValue = FLT_MAX;
	}


	return nndists;
}
