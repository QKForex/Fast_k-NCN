#include "Classifier.h"

//
//	Find first Nearest Neighbor - simplified version 
//
//	Input: list of precomputed distances for given sample
//	Output: list of k nearest neighbors
//
const Distance Classifier::find1NN(Distance* dists, int distsSize) {
	Distance nndists;
	nndists = dists[0];	
	for (int distsIndex = 1; distsIndex < distsSize; distsIndex++) {
		if (dists[distsIndex].distValue < nndists.distValue) {
			nndists = dists[distsIndex];
		}
	}
	return nndists;
}

//	
//	Assigns sample to most frequent label
//	from k nearest neighbors found by findkNN function
//
//	Input:
//	Output:
// 
int Classifier::assignLabel(const Distance* dists, int distsSize)
{
	if (distsSize == 1) { return dists[0].sampleLabel; }

	//  first find largest class label
	// 	create array of distsSize of the greatest class label so that we can increase
	//	the value under that index every time that label is met
	int i = 0;
	int largestLabel = dists[i].sampleLabel;
	for (i = 1; i < distsSize; i++) {
		if (largestLabel < dists[i].sampleLabel) {
			largestLabel = dists[i].sampleLabel;
		}
	}
	
	int* freqs = new int[largestLabel + 1];
	for (i = 0; i < largestLabel + 1; i++) { freqs[i] = 0; }

	int result = 0;
	for (i = 0; i < distsSize; i++)	{
		// in case of draw choose the label with smallest distance,
		// change ">" to ">=" to choose the last, largest
		if (++freqs[dists[i].sampleLabel] > freqs[result]) {
			result = dists[i].sampleLabel;
		}
	}

	delete[] freqs;
	delete[] dists;

	return result;
}

int Classifier::countError(int* const& result, const SampleSet& orig) {
	const int origSize = orig.getNrSamples();
	int error = 0;

	// comments in this function - uncomment for more verbose logging purposes
	//ofstream file("result.txt");
	for (int origIndex = 0; origIndex < origSize; origIndex++) {
		//file << orig[origIndex].getLabel() << " " << result[origIndex];
		if (orig[origIndex].getLabel() != result[origIndex])
		{
			error++;
			//file << " error";
		}
		//file << "\n";
	}
	//file << "Number of errors: " << error << endl;
	return error;
}