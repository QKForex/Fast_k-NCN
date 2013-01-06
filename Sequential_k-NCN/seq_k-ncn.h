#pragma once
#ifndef SEQ_K_NCN_H_
#define SEQ_K_NCN_H_


#include <algorithm>
#include <climits>

#include <time.h>
#include <windows.h>
#include "Fast_k-NCN_Common.h"

using namespace Common;
using namespace std;

const Distance find1NN(const Distance* dists, int distsSize)
{
	Distance nndists;
	nndists = dists[0];	

	for (int distsIndex = 1; distsIndex < distsSize; distsIndex++) {
		if (dists[distsIndex].distValue < nndists.distValue) {
			nndists = dists[distsIndex];
		}
	}

	delete[] dists;

	return nndists;
}

/*
 *	Find k Nearest Centroid Neighbors 
 *
 *	Input: list of precomputed distances for given sample
 *	Output: list of k nearest centroid neighbors
 */
const Distance* findkNN(const Distance* dists, int distsSize, int k)
{
	// sorted list of k NNs (ascending, smallest first)
	Distance* nndists = (Distance*) malloc(k * sizeof(Distance));
	// initialize with max float
	fill(nndists, nndists+k, Distance(-1,-1, FLT_MAX));

	// remaining NNs from dists list
	for (int distsIndex = 0; distsIndex < distsSize; distsIndex++) {
		// if current neighbor is closer than the furthest one
		// in the list find a place for current NN
		if (dists[distsIndex].distValue < nndists[k - 1].distValue) {
			// for every NN that is in the nndist list already
			// find suitable position in list (starting from the furthest)
			for (int distsInNndistsIndex = 0; distsInNndistsIndex < k; distsInNndistsIndex++) {
				// if the distance is smaller than distance of the current NN
				// to be added, move the NN in list
				if (dists[distsIndex].distValue < nndists[distsInNndistsIndex].distValue) {
					// move NNs to make place for current new NN
					// this should happen just once
					for (int distsToMoveIndex = k - 1; distsToMoveIndex > distsInNndistsIndex; distsToMoveIndex--) {
						nndists[distsToMoveIndex] = nndists[distsToMoveIndex - 1];
					}
					
					nndists[distsInNndistsIndex] = dists[distsIndex];
					break;
				}
			}
		}
	}

	delete[] dists; //TODO Should get rid off this delete

	return nndists;
}

/*	
 *	Assigns sample to most frequent label
 *	from k nearest neighbors found by findkNN function
 *
 *	Input:
 *	Output:
 */
int assignLabel(const Distance* dists, int distsSize)
{
	/*  first find largest class label
	 * 	create array of distsSize of the greatest class label so that we can increase
	 *  the value under that index every time that label is met
	 */

	if (distsSize == 1)
		return dists[0].sampleLabel;

	int i = 0;
	int largestLabel = dists[i].sampleLabel;
	for (i = 1; i < distsSize; i++)
		if (largestLabel < dists[i].sampleLabel)
			largestLabel = dists[i].sampleLabel;

	int* freqs = new int[largestLabel + 1];

	for (i = 0; i < largestLabel + 1; i++)
		freqs[i] = 0;

	int result = 0;
	for (i = 0; i < distsSize; i++)
	{
		/* in case of draw choose the label with smallest distance,
		 * change ">" to ">=" to choose the last, largest
		 */

		if (++freqs[dists[i].sampleLabel] > freqs[result])
		{
			result = dists[i].sampleLabel;
		}
	}

	delete[] freqs;
	delete[] dists;

	return result;
}

int classify(const SampleSet& trainSet, const Sample& testSam, const int k) // wrapper function
{
	const int trainSize = trainSet.getNrSamples();
	if (k == 1)
		return find1NN(countDistances(trainSet, testSam), trainSize).sampleLabel;
	else
		return assignLabel(findkNN(countDistances(trainSet, testSam), trainSize, k), k);
}

int countError(int* const& result, const SampleSet& orig)
{
	const int origSize = orig.getNrSamples();
	int error = 0;

	// comments in this function - uncomment for more verbose logging purposes
	//ofstream file("result.txt");

	for (int origIndex = 0; origIndex < origSize; origIndex++)
	{
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


#endif /* SEQ_K_NCN_H_ */