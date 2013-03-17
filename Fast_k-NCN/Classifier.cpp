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
//	Find best performing classifier on given dataset
//	this method is bound with dataset
//	TODO: Possibly will move this method to other class so that it constructs classifier
//	every time, there will be no need for complex interfaces anymore
//	it could simply use preprocess and classify	
//	Possibly should stay as a Classifier method, because it is going to be used in other
//	as a part of algorithm
//
//	Input:
//	Output:
//
const int Classifier::learnOptimalK(const SampleSet& trainSet, const int largestK) {
	std::vector< std::pair<int,int> > errorsForK;

	Distance** trainDists = new Distance*[trainSet.nrSamples];
	for (int i = 0; i < trainSet.nrSamples; i++) {
		trainDists[i] = new Distance[trainSet.nrSamples - 1];
		std::fill(trainDists[i], trainDists[i]+(trainSet.nrSamples-1), Distance(-1,-1, FLT_MAX));
	}

	SampleSet remainingTrainSamples(trainSet);
	// last position in array is leave-one-out sample
	// change the number of samples to make methods treat remainingTrainSamples
	// as it would have one less sample hiding the leave-one-out sample
	remainingTrainSamples.nrSamples = remainingTrainSamples.nrSamples - 1;

	// preprocess
	// count distances between all training samples
	for (int samIndex = 0; samIndex < remainingTrainSamples.nrSamples; samIndex++) {
		remainingTrainSamples.swapSamples(samIndex, trainSet.nrSamples - 1);
		Common::countDistances(remainingTrainSamples,
			remainingTrainSamples[trainSet.nrSamples - 1], trainDists[samIndex]);
		remainingTrainSamples.swapSamples(trainSet.nrSamples - 1, samIndex);
	}
	// for last sample without swap
	Common::countDistances(remainingTrainSamples, remainingTrainSamples[trainSet.nrSamples - 1],
		trainDists[trainSet.nrSamples - 1]);

	// classify
	// using all values k betwen 1 and largestK
	// and collect number of errors
	// for every kIndex check all leave-one-out combinations in trainSet
	// for every sample perform leave-one-out and increase counter on error
	for (int kIndex = 1; kIndex <= largestK; kIndex++) {

		std::pair<int, int> p(kIndex,0);
		errorsForK.push_back(p);

		Distance** trainNNdists = new Distance*[trainSet.nrSamples];
		for (int i = 0; i < trainSet.nrSamples; i++) {
			trainNNdists[i] = new Distance[kIndex];
			std::fill(trainNNdists[i], trainNNdists[i]+kIndex, Distance(-1,-1, FLT_MAX));
		}

		for (int samIndex = 0; samIndex < remainingTrainSamples.nrSamples; samIndex++) {
			remainingTrainSamples.swapSamples(samIndex, trainSet.nrSamples - 1);
			int label = classifySample(remainingTrainSamples, 
				remainingTrainSamples[trainSet.nrSamples - 1],
				trainDists[samIndex], trainNNdists[samIndex], kIndex);
			if (label != remainingTrainSamples[trainSet.nrSamples - 1].label) {
				errorsForK[kIndex-1].second++;
			}
			remainingTrainSamples.swapSamples(trainSet.nrSamples - 1, samIndex);
		}
		// for last sample without swap
		int label = classifySample(remainingTrainSamples,
			remainingTrainSamples[trainSet.nrSamples - 1],
			trainDists[trainSet.nrSamples - 1], trainNNdists[trainSet.nrSamples - 1], kIndex);
		if (label != remainingTrainSamples[trainSet.nrSamples - 1].label) {
			errorsForK[kIndex-1].second++;
		}
			
		for (int i = 0; i < trainSet.nrSamples; i++) { delete trainNNdists[i]; }
		delete[] trainNNdists;
	}

	// sort so the classifer with smallest number of errors is first
	std::sort(errorsForK.begin(), errorsForK.end(), 
		boost::bind(&std::pair<int, int>::second, _1) <
		boost::bind(&std::pair<int, int>::second, _2));

	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) { 
		delete trainDists[distIndex];
	}
	delete[] trainDists;

	return errorsForK[0].first;
}

int Classifier::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, this->distances[testSample.index],
		this->nndists[testSample.index], this->k);
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
								   const Distance* testSampleDists) {							   
	Distance nearestNeighbourDist = testSampleDists[0];
	//TODO: to parallelize somehow, e.g. compare in 4 threads,
	// and then compare 4 results in the end
	for (int distsIndex = 1; distsIndex < trainSet.nrSamples; distsIndex++) {
		if (testSampleDists[distsIndex].distValue < nearestNeighbourDist.distValue) {
			nearestNeighbourDist = testSampleDists[distsIndex];
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
int Classifier::assignLabel(const Distance* testSampleNNdists, const int k) {
	if (k == 1) { return testSampleNNdists[0].sampleLabel; }

	//  first find largest class label
	// 	create array of distsSize of the greatest class label so that we can increase
	//	the value under that index every time that label is met
	int i = 0;
	int largestLabel = testSampleNNdists[i].sampleLabel;
	for (i = 1; i < k; i++) {
		if (largestLabel < testSampleNNdists[i].sampleLabel) {
			largestLabel = testSampleNNdists[i].sampleLabel;
		}
	}
	
	int* freqs = new int[largestLabel + 1]; //TODO: change to std::vector
	std::fill(freqs, freqs+largestLabel+1, 0);

	int result = 0;
	for (i = 0; i < k; i++)	{
		// in case of draw choose the label with smallest distance,
		// change ">" to ">=" to choose the last, largest
		if (++freqs[testSampleNNdists[i].sampleLabel] > freqs[result]) {
			result = testSampleNNdists[i].sampleLabel;
		}
	}

	delete[] freqs;

	return result;
}

int Classifier::assignLabel(const int testSampleIndex) {
	return assignLabel(this->distances[testSampleIndex], this->k);
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
