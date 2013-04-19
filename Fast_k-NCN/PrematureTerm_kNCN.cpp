#include "PrematureTerm_kNCN.h"


PrematureTerm_kNCN::PrematureTerm_kNCN(): Classifier() {} 

PrematureTerm_kNCN::PrematureTerm_kNCN(const int k,
									   const int nrTrainSamples,
									   const int nrTestSamples,
									   const int threshold)
									   : Classifier(k, nrTrainSamples, nrTestSamples) {
	this->threshold = threshold;
}

PrematureTerm_kNCN::~PrematureTerm_kNCN() {
	if (!results) { delete[] results; }
	if (!nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
}

void PrematureTerm_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
void PrematureTerm_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		results[samIndex] = classifySample(trainSet, testSet[samIndex]);
	}
}

int PrematureTerm_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	 

		if (k == 1) {
			return find1NNPrematureTerm(trainSet, testSample).sampleLabel;
		} else {
			findkNCNPrematureTerm(const_cast<SampleSet&> (trainSet), testSample, testSampleNNdists, k);
			return assignLabel(testSampleNNdists, k);
		}

}

int PrematureTerm_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, nullptr,
		this->nndists[testSample.index], this->k);
}

void PrematureTerm_kNCN::countDistancePrematureTerm(const Sample& trainSample, const Sample& testSample,
	Distance* distances, int samIndex, int threshold) {
	distances[samIndex].sampleIndex = samIndex;
	distances[samIndex].sampleLabel = trainSample.label;
#ifdef MANHATTAN_DIST
	distances[samIndex].distValue = countManhattanDistance(trainSample, testSample, threshold);
#elif EUCLIDEAN_DIST
	distances[samIndex].distValue = countEuclideanDistance(trainSet[samIndex], testSample, threshold);
#endif
}

Distance PrematureTerm_kNCN::countDistancePrematureTerm(const Sample& trainSample, const Sample& testSample, int threshold) {
	Distance resultDist;
	resultDist.sampleIndex = trainSample.index;
	resultDist.sampleLabel = trainSample.label;
#ifdef MANHATTAN_DIST
	resultDist.distValue = countManhattanDistance(trainSample, testSample, threshold);
#elif EUCLIDEAN_DIST
	resultDist.distValue = countEuclideanDistance(trainSet[samIndex], testSample, threshold);
#endif
	return resultDist;
}

const Distance PrematureTerm_kNCN::find1NNPrematureTerm(const SampleSet& trainSet, const Sample& testSample) {
	Distance candidateNNdist;
	Distance nearestNeighbourDist = countDistancePrematureTerm(trainSet[0], testSample, testSample.nrDims);
	for (int distsIndex = 1; distsIndex < trainSet.nrSamples; distsIndex++) {
		candidateNNdist = countDistancePrematureTerm(trainSet[distsIndex], testSample, this->threshold);
		if (candidateNNdist.distValue < nearestNeighbourDist.distValue) {
			countDistancePrematureTerm(trainSet[distsIndex], testSample, testSample.nrDims);
			if (candidateNNdist.distValue < nearestNeighbourDist.distValue) {
				nearestNeighbourDist = candidateNNdist;
			}
		}
	}
	return nearestNeighbourDist;
}

void PrematureTerm_kNCN::findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample,
										  Distance* testSampleNNdists, const int k) {
	SampleSet centroids(trainSet.nrClasses, trainSet.nrDims, k); //TODO: move to attributes for Seq_kNCN
	
	testSampleNNdists[0] = find1NNPrematureTerm(trainSet, testSample);
	centroids[0] = trainSet[testSampleNNdists[0].sampleIndex];
	//trainSet[testSampleNNdists[0].sampleIndex].swap(trainSet[trainSet.nrSamples - 1]);
	trainSet.swapSamples(testSampleNNdists[0].sampleIndex, trainSet.nrSamples - 1);

	int trainSetNrDims = trainSet.nrDims;

	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		Sample currentCentroid(-1, -1, trainSet.nrDims);
		float divCentroidIndex = 1.0f / (centroidIndex+1);
		int samIndexLimit = trainSet.nrSamples - centroidIndex;
		for (int samIndex = 0; samIndex < samIndexLimit; samIndex++) {
			for (int dimIndex = 0; dimIndex < trainSetNrDims; dimIndex++) {
				currentCentroid[dimIndex] =	((centroids[centroidIndex-1][dimIndex] * centroidIndex)
					+ (trainSet[samIndex][dimIndex])) * divCentroidIndex; //TODO: bootleneck!

				//currentCentroid[dimIndex] =	(centroids[centroidIndex-1][dimIndex] * ((float)centroidIndex/(centroidIndex+1)))
				//	+ (trainSet[samIndex][dimIndex] / (centroidIndex+1));
				//currentCentroid[dimIndex] =	centroids[centroidIndex-1][dimIndex] + trainSet[samIndex][dimIndex];
			}
			
			Distance currentNNdist = countDistancePrematureTerm(currentCentroid, testSample, this->threshold);
			
			if (currentNNdist.distValue < testSampleNNdists[centroidIndex].distValue) {
				currentNNdist = countDistancePrematureTerm(currentCentroid, testSample, testSample.nrDims);

				if (currentNNdist.distValue < testSampleNNdists[centroidIndex].distValue) {
					currentCentroid.index = trainSet[samIndex].index; // do not need to do that
					currentCentroid.label = trainSet[samIndex].label;
					currentNNdist.sampleIndex = trainSet[samIndex].index;
					currentNNdist.sampleLabel = trainSet[samIndex].label;
					testSampleNNdists[centroidIndex] = currentNNdist;
					centroids[centroidIndex] = currentCentroid;
				}
			}
		}
		//trainSet[testSampleNNdists[centroidIndex].sampleIndex].swap(trainSet[trainSet.nrSamples-1 - centroidIndex]);
		trainSet.swapSamples(testSampleNNdists[centroidIndex].sampleIndex, trainSet.nrSamples-1 - centroidIndex);
	}
}

void PrematureTerm_kNCN::findkNCNPrematureTerm(SampleSet& trainSet, const Sample& testSample) {
	return findkNCNPrematureTerm(trainSet, testSample, this->nndists[testSample.index], this->k);
}