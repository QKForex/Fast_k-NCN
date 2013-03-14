#include "Classifier.h"

Classifier::Classifier() : k(1), nrTrainSamples(0), nrTestSamples(0), nrClassificationErrors(0), errorRate(0.0f) {
	distances = nullptr;
	results = nullptr;
	nndists = nullptr;
}

Classifier::Classifier(const int k, const int nrTrainSamples, const int nrTestSamples)
	: k(k), nrTrainSamples(nrTrainSamples), nrTestSamples(nrTestSamples), nrClassificationErrors(0), errorRate(0.0f) {
	distances = new Distance*[nrTestSamples];
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) {
		distances[distIndex] = new Distance[nrTrainSamples];
		std::fill(distances[distIndex], distances[distIndex]+nrTrainSamples, Distance(-1,-1, FLT_MAX));
	}

	nndists = new Distance*[nrTestSamples];
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) {
		nndists[distIndex] = new Distance[k];
		std::fill(nndists[distIndex], nndists[distIndex]+k, Distance(-1,-1, FLT_MAX));
	}

	
	results = new int[nrTestSamples](); //TODO: should it be initialized to 0?
}

//
//	Find optimal k - number of nearest nieghbors using leave-one-out method
//	
//	Input:
//	Output:
//
const int Classifier::learnOptimalK(const SampleSet& trainSet, const int largestK) {
	std::vector< std::pair<int,int> > errorsForK;

	Distance** nndists = new Distance*[trainSet.nrSamples];
	for (int i = 0; i < trainSet.nrSamples; i++) {
		nndists[i] = new Distance[trainSet.nrSamples - 1];
	}

	SampleSet remainingTrainSamples(trainSet); //TODO: change this classifier
	remainingTrainSamples.nrSamples = remainingTrainSamples.nrSamples - 1; //TODO: change!
	// na koncu jest 1399, 0, 1, 2,..., 1398
	
	for (k = 1; k <= largestK; k++) {

		//Sample currentTrainSample(remainingTrainSamples[remainingTrainSamples.nrSamples - 1]);
		// for every k check all leave-one-out combinations in trainSet
		std::pair<int, int> p(k,0);
		errorsForK.push_back(p);

		for (int samIndex = 0; samIndex < remainingTrainSamples.nrSamples - 1; samIndex++) {
			// for every sample perform leave-one-out and increase counter on error
			remainingTrainSamples.swapSamples(samIndex, trainSet.nrSamples - 1);
			//swapSamples(remainingTrainSamples[samIndex], currentTrainSample);
			countDistances(remainingTrainSamples, remainingTrainSamples[trainSet.nrSamples - 1], nndists[samIndex]);
			int label = classifySample(remainingTrainSamples, remainingTrainSamples[trainSet.nrSamples - 1], nndists[samIndex]);
			if (label != remainingTrainSamples[trainSet.nrSamples - 1].label) {
				errorsForK[k-1].second++;
			}
			remainingTrainSamples.swapSamples(trainSet.nrSamples - 1, samIndex);
			//swapSamples(currentTrainSample, remainingTrainSamples[samIndex]);
		}

		countDistances(remainingTrainSamples, remainingTrainSamples[trainSet.nrSamples - 1], nndists[trainSet.nrSamples - 1]);
		int label = classifySample(remainingTrainSamples, remainingTrainSamples[trainSet.nrSamples - 1], nndists[trainSet.nrSamples - 1]);
		if (label != remainingTrainSamples[trainSet.nrSamples - 1].label) {
			errorsForK[k-1].second++;
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

int Classifier::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, distances[testSample.index]);
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

void Classifier::calculateErrorRate(const SampleSet& orig) {
	nrClassificationErrors = calculateError(orig, results);	
	errorRate = (float) nrClassificationErrors / orig.nrSamples * 100;
}

//
//	Find first Nearest Neighbor - simplified version 
//	The resultant nearest neighbor consists of label
//
//	Input: list of precomputed distances for given sample
//	Output: nearest neighbor
//
const Distance Classifier::find1NN(const SampleSet& trainSet, const Sample& testSample,
								   Distance* dists) {							   
	Distance nearestNeighbourDist = dists[0];
	//TODO: to parallelize somehow, e.g. compare in 4 threads,
	// and then compare 4 results in the end
	for (int distsIndex = 1; distsIndex < trainSet.nrSamples; distsIndex++) {
		if (dists[distsIndex].distValue < nearestNeighbourDist.distValue) {
			nearestNeighbourDist = dists[distsIndex];
		}
	}
	return nearestNeighbourDist;
}

//
//	Wrapper for find1NN that uses distances attribute instead of specified distances
//
const Distance Classifier::find1NN(const SampleSet& trainSet, const Sample& testSample) {
	return find1NN(trainSet, testSample, distances[testSample.index]);
}

//	
//	Assigns sample to most frequent label
//	from k nearest neighbors found by findkNN function
//
//	Input:
//	Output:
// 
int Classifier::assignLabel(const int testSampleIndex) {
	if (k == 1) { return nndists[testSampleIndex][0].sampleLabel; }

	//  first find largest class label
	// 	create array of distsSize of the greatest class label so that we can increase
	//	the value under that index every time that label is met
	int i = 0;
	int largestLabel = nndists[testSampleIndex][i].sampleLabel;
	for (i = 1; i < k; i++) {
		if (largestLabel < nndists[testSampleIndex][i].sampleLabel) {
			largestLabel = nndists[testSampleIndex][i].sampleLabel;
		}
	}
	
	int* freqs = new int[largestLabel + 1];
	for (i = 0; i < largestLabel + 1; i++) { freqs[i] = 0; }

	int result = 0;
	for (i = 0; i < k; i++)	{
		// in case of draw choose the label with smallest distance,
		// change ">" to ">=" to choose the last, largest
		if (++freqs[nndists[testSampleIndex][i].sampleLabel] > freqs[result]) {
			result = nndists[testSampleIndex][i].sampleLabel;
		}
	}

	delete[] freqs;

	return result;
}

int Classifier::calculateError(const SampleSet& orig, const int* results) {
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
