#include "Classifier.h"
#include <vector> //TODO should go from here
#include <algorithm>
#include <boost\bind.hpp>

Classifier::Classifier() : k(1), nrTrainSamples(0), nrTestSamples(0) {
	distances = NULL;
	results = NULL;
}

Classifier::Classifier(const int k, const int nrTrainSamples, const int nrTestSamples)
	: k(k), nrTrainSamples(nrTrainSamples), nrTestSamples(nrTestSamples) {
	distances = NULL;
	results = NULL;
}

//
//	Find optimal k - number of nearest nieghbors using leave-one-out method
//	
//	Input:
//	Output:
//
const int Classifier::learnOptimalK(const SampleSet& trainSet, const int largestK) {
	vector< pair<int,int> > errorsForK;

	Distance** nndists = new Distance*[nrTestSamples];


	for (k = 1; k <= largestK; k++) {
		SampleSet remainingTrainSamples(trainSet);
		remainingTrainSamples.nrSamples = remainingTrainSamples.nrSamples - 1;

		Sample currentTrainSample(remainingTrainSamples[remainingTrainSamples.nrSamples - 1]);

		// for every k check all leave-one-out combinations in trainSet
		pair<int, int> p(k,0);
		errorsForK.push_back(p);
		for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			// for every sample perform leave-one-out and increase counter on error
			remainingTrainSamples.swapSamples(samIndex, trainSet.nrSamples - 1);
			nndists[samIndex] = countDistances(remainingTrainSamples, currentTrainSample);
			int label = classifySample(remainingTrainSamples, currentTrainSample, nndists[samIndex]);
			if (label != currentTrainSample.label) {
				errorsForK[k].second++;
			}
		}
	}

	std::sort(errorsForK.begin(), errorsForK.end(), 
          boost::bind(&std::pair<int, int>::second, _1) <
          boost::bind(&std::pair<int, int>::second, _2));

	const int optimalK = errorsForK[0].first;

	for (int i = 0; i < nrTestSamples; i++) { delete nndists[i]; }
	delete[] nndists;

	return optimalK;
}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
void Classifier::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
		for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
			results[samIndex] = classifySample(trainSet, testSet[samIndex]);
		}
}

//
//	Find first Nearest Neighbor - simplified version 
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest neighbors
//
const Distance Classifier::find1NN(const SampleSet& trainSet, const Sample& testSample,
								   Distance** dists) {							   
	Distance nearestNeighbourDist = dists[testSample.index][0];
	//TODO: to parallelize somehow, e.g. compare in 4 threads,
	// and then compare 4 results in the end
	for (int distsIndex = 1; distsIndex < trainSet.nrSamples; distsIndex++) {
		if (dists[testSample.index][distsIndex].distValue < nearestNeighbourDist.distValue) {
			nearestNeighbourDist = dists[testSample.index][distsIndex];
		}
	}
	return nearestNeighbourDist;
}

//
//	Wrapper for find1NN that uses distances attribute instead of specified distances
//
const Distance Classifier::find1NN(const SampleSet& trainSet, const Sample& testSample) {
	return find1NN(trainSet, testSample, distances);
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

int Classifier::calculateError(const int* results, const SampleSet& orig) {
	int nrError = 0;
	// comments in this function - uncomment for more verbose logging purposes
	//ofstream file("result.txt");
	for (int origIndex = 0; origIndex < orig.nrSamples; origIndex++) {
		//file << orig[origIndex].getLabel() << " " << result[origIndex];
		if (orig[origIndex].label != results[origIndex]) {
			nrError++;
			//file << " error";
		}
		//file << std::endl;
	}
	//file << "Number of errors: " << nrError << std::endl;
	return nrError;
}

void Classifier::calculateErrorRate(const SampleSet& orig) {
	nrClassificationErrors = calculateError(results, orig);	
	errorRate = (float) nrClassificationErrors / orig.nrSamples * 100;
}

int Classifier::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, distances[testSample.index]);
}