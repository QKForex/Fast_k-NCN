#include "Sequential_kNCN.h"

Sequential_kNCN::Sequential_kNCN() {}

Sequential_kNCN::~Sequential_kNCN() {}

Distance** Sequential_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	const int nrTrainSamples = trainSet.getNrSamples();
	const int nrTestSamples = testSet.getNrSamples();

	Distance** distancesToTestSamples = new Distance*[nrTestSamples];
	
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		distancesToTestSamples[samIndex] = countDistances(trainSet, testSet[samIndex]);
	}
	
	return distancesToTestSamples;
}

int* Sequential_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet, const int k, Distance** dists) {
	const int nrTrainSamples = trainSet.getNrSamples();
	const int nrTestSamples = testSet.getNrSamples();		  
	int* results = new int[nrTestSamples];

	if (k == 1) {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = find1NN(dists[samIndex], nrTrainSamples).sampleLabel;
		}
	} else {
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = assignLabel(findkNCN(dists[samIndex], nrTrainSamples, k), k);
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
//	we set the distance to FLT_MAX for chosen kNCN sample in dists once
//	this excludes first kNCN from being chosen again
//	this action will be repeated for the rest 
//	dists[nndists[0].sampleIndex].distValue = FLT_MAX;
//	check if given sample is not kNCN already
//
//TODO inne rozw. ew. mozna zamienic z ostatnim i iterowaæ po n - j

const Distance* Sequential_kNCN::findkNCN(Distance* dists, int distsSize, int k) {
	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));

	//Sample* centroids = (Sample*) malloc(k * sizeof(Sample));
	DistanceValue* centroids = (DistanceValue*) malloc(k * sizeof(DistanceValue));
	nndists[0] = find1NN(dists, distsSize);
	centroids[0] = nndists[0].distValue;
	DistanceValue* tempCentroids = (DistanceValue*) malloc(distsSize * sizeof(DistanceValue));
	DistanceValue* tempCentroidsDistances = (DistanceValue*) malloc(distsSize * sizeof(DistanceValue));
	DistanceValue sum = 0;
	Distance minCentroidDistance = Distance(-1,-1,FLT_MAX);
	bool iskNCN = false;
	int sampleDims = 30; //TODO hardcoded
	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		sum = 0;
		//minCentroidDistance = Sample(-1, sampleDims, ;
		for (int previousCentroidIndex = 0; previousCentroidIndex < centroidIndex; previousCentroidIndex++) {
			sum += centroids[previousCentroidIndex];
		}
		for (int distsIndex = 0; distsIndex < distsSize; distsIndex++) {
			iskNCN = false;
			for (int i = 0; i < centroidIndex; i++) {
				if (nndists[i].sampleIndex == distsIndex) {
					iskNCN = true;
					break;
				}
			}



			if (!iskNCN) {			
				tempCentroids[distsIndex] = (dists[distsIndex].distValue + sum)/centroidIndex;
				tempCentroidsDistances[distsIndex] =
					(dists[distsIndex].distValue - tempCentroids[distsIndex]) * 
					(dists[distsIndex].distValue - tempCentroids[distsIndex]);
				if (minCentroidDistance.distValue > tempCentroidsDistances[distsIndex]) {
					minCentroidDistance = dists[distsIndex];
					minCentroidDistance.distValue = tempCentroidsDistances[distsIndex];
				}
			}

		}

		nndists[centroidIndex] = dists[minCentroidDistance.sampleIndex];

		//dists[nndists[centroidIndex].sampleIndex].distValue = FLT_MAX;
	}


	return nndists;
}
