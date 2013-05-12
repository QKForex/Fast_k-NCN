#include "LimitedV2_kNCN.h"

LoggerPtr LimitedV2_kNCN::logger(Logger::getLogger("LimV2_kNCNLogger"));

LimitedV2_kNCN::LimitedV2_kNCN() : Classifier(), centroids(SampleSet(-1, -1, k)) {}

LimitedV2_kNCN::LimitedV2_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples, 
								 const int nrClasses, const int nrDims, const float percentMaxRobustRank)
	: Classifier(k, nrTrainSamples, nrTestSamples), centroids(SampleSet(nrClasses, nrDims, k)), percentMaxRobustRank(percentMaxRobustRank) {
	maximalRobustRanks = new int[k];
	distances = new Distance*[nrTestSamples];
	for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) {
		distances[distIndex] = new Distance[nrTrainSamples];
		std::fill(distances[distIndex], distances[distIndex]+nrTrainSamples, Distance(-1,-1, FLT_MAX));
	}

	for (int centroidIndex = 0; centroidIndex < k; centroidIndex++) {
		centroids[centroidIndex].nrDims = nrDims;
		centroids[centroidIndex].dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
	}
}

LimitedV2_kNCN::~LimitedV2_kNCN() {
	if (results) { delete[] results; }
	if (nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
	if (distances) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
		delete[] distances;
	}
	if (maximalRobustRanks) { delete[] maximalRobustRanks; }
}

void LimitedV2_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {
	learnMRobustRanks(const_cast<SampleSet&> (trainSet));
	
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], distances[samIndex]);
	}
}

void LimitedV2_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		results[samIndex] = classifySample(trainSet, testSet[samIndex],
			distances[samIndex], nndists[samIndex], k);
	}
}

int LimitedV2_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists, const int k) {	  								   
	if (k == 1) {
		return find1NNLimited(trainSet, testSample, testSampleDists, this->maximalRobustRanks[0]).sampleLabel;
	} else {
		findkNCNLimitedV2(trainSet, testSample, testSampleDists, testSampleNNdists, k);
		//LOG4CXX_DEBUG(logger, "" << testSampleNNdists[0].sampleIndex << " " << testSampleNNdists[0].distValue
		//	<< " " << testSampleNNdists[1].sampleIndex << " " << testSampleNNdists[1].distValue
		//	<< " " << testSampleNNdists[2].sampleIndex << " " << testSampleNNdists[2].distValue
		//	<< " " << testSampleNNdists[3].sampleIndex << " " << testSampleNNdists[3].distValue
		//	<< " " << testSampleNNdists[4].sampleIndex << " " << testSampleNNdists[4].distValue
		//	);
		return assignLabel(testSample.index);
	}
}

const Distance LimitedV2_kNCN::find1NNLimited(const SampleSet& trainSet, const Sample& testSample,
	const Distance* testSampleDists, const int maximalRobustRank) {
	Distance nearestNeighbourDist = testSampleDists[0];
	for (int distsIndex = 1; distsIndex < maximalRobustRank; distsIndex++) {
		if (testSampleDists[distsIndex].distValue < nearestNeighbourDist.distValue) {
			nearestNeighbourDist = testSampleDists[distsIndex];
		}
	}
	return nearestNeighbourDist;
}

void LimitedV2_kNCN::findkNCNLearn(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists, const int k) {
	int trainSetNrDims = trainSet.nrDims;
#if defined SSE
	int registersNumber = (trainSet.nrDims >> 2) + 1;
	union xmmregister
	{
		__m128 m;
		DistanceValue f[4];
	} result;
#elif defined AVX
	int registersNumber = (trainSet.nrDims >> 3) + 1;
	union immregister
	{
		__m256 m;
		DistanceValue f[8];
	} result;
#endif

	testSampleNNdists[0] = Classifier::find1NN(trainSet, testSample, testSampleDists);
	centroids[0] = trainSet[testSampleNNdists[0].sampleIndex];
	swapDistances(testSampleDists, testSampleNNdists[0].sampleIndex, trainSet.nrSamples-1);

	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		// current centroid candidate
		// centroid does not have index or label, just dimensions
		Sample* currentCentroid = &centroids[centroidIndex]; 
		Sample closestCentroid;
		Sample* previousCentroid = &centroids[centroidIndex-1];

		int samIndexLimit = trainSet.nrSamples - centroidIndex;
		int currentSamPos = -1;
		float divCentroidIndex = 1.0f / (centroidIndex+1);

#if defined SSE
		const __m128i centroidIndexISrc = _mm_set1_epi32(centroidIndex);
		const __m128 centroidIndexSrc = (__m128) _mm_cvtepi32_ps(centroidIndexISrc);
		const __m128 divCentroidIndexSrc = (__m128) _mm_set1_ps(divCentroidIndex);
#elif defined AVX
		const __m256i centroidIndexISrc = _mm256_set1_epi32(centroidIndex);
		const __m256 centroidIndexSrc = (__m256) _mm256_cvtepi32_ps(centroidIndexISrc);
		const __m256 divCentroidIndexSrc = (__m256) _mm256_set1_ps(divCentroidIndex);
#endif

		for (int samIndex = 0; samIndex < samIndexLimit; samIndex++) {
			const Sample* trainSample = &trainSet[testSampleDists[samIndex].sampleIndex];

#if defined SSE	
			const __m128* currentCentroidSrc = (__m128*) currentCentroid->dims;
			const __m128* previousCentroidSrc = (__m128*) previousCentroid->dims;
			const __m128* trainSampleSrc = (__m128*) trainSample->dims;

			result.m = _mm_set1_ps(0x0);
			for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
				result.m = _mm_mul_ps(*previousCentroidSrc, centroidIndexSrc);
				result.m = _mm_add_ps(result.m, *trainSampleSrc);
				result.m = _mm_mul_ps(result.m, divCentroidIndexSrc);	

				_mm_store_ps(&(currentCentroid->dims[registerIndex*4]), result.m);	

				currentCentroidSrc++;
				previousCentroidSrc++;
				trainSampleSrc++;
			}
#elif defined AVX
			const __m256* currentCentroidSrc = (__m256*) currentCentroid->dims;
			const __m256* previousCentroidSrc = (__m256*) previousCentroid->dims;
			const __m256* trainSampleSrc = (__m256*) trainSample->dims;

			result.m = _mm256_set1_ps(0x0);
			for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
				result.m = _mm256_mul_ps(*previousCentroidSrc, centroidIndexSrc);
				result.m = _mm256_add_ps(result.m, *trainSampleSrc);
				result.m = _mm256_mul_ps(result.m, divCentroidIndexSrc);

				_mm256_store_ps(&(currentCentroid->dims[registerIndex*8]), result.m);

				currentCentroidSrc++;
				previousCentroidSrc++;
				trainSampleSrc++;
			}
#else
			for (int dimIndex = 0; dimIndex < trainSetNrDims; dimIndex++) {
				currentCentroid->dims[dimIndex] = ((previousCentroid->dims[dimIndex] * centroidIndex) + trainSample->dims[dimIndex]) * divCentroidIndex;
			}
#endif
			DistanceValue currentNCNdistVal = countManhattanDistance(*currentCentroid, testSample, 0, trainSet.nrDims); //TODO: hardcoded Manhattan

			if (currentNCNdistVal < testSampleNNdists[centroidIndex].distValue && testSampleDists[samIndex].sampleIndex != testSample.index) {
				// update current centroid candidate
				closestCentroid = *currentCentroid;
				
				// update data structure that keeps track of the trainSample that give the current nearest centroid
				testSampleNNdists[centroidIndex] =
					Distance(testSampleDists[samIndex].sampleIndex, testSampleDists[samIndex].sampleLabel, currentNCNdistVal);

				// update current trainSample postion in trainSet (because it could have been changed)
				// for situation when a just swapped trainSample is chosen
				currentSamPos = samIndex;
			}
		}
		// finally assign closestCentroid to return array 
		centroids[centroidIndex] = closestCentroid;
		
		// move current trainSample that gives the current nearest centroid to the end of trainSet
		// so it is not taken into account anymore
		swapDistances(testSampleDists, currentSamPos, (trainSet.nrSamples-1) - centroidIndex);
	}
}

void LimitedV2_kNCN::findkNCNLearn(const SampleSet& trainSet, const Sample& testSample) {
	findkNCNLearn(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}

void LimitedV2_kNCN::findkNCNLimitedV2(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists, const int k) {
	// find the fraction
	randomizedSelect(testSampleDists, 0, trainSet.nrSamples-1, maximalRobustRanks[k-1]);
	std::qsort(testSampleDists, maximalRobustRanks[k-1], sizeof(Distance), compareDistances); // the closest need to be sorted

	// then use the fraction of trainSamples to classify the testSamples in classification part
	int trainSetNrDims = trainSet.nrDims;
#if defined SSE
	int registersNumber = (trainSet.nrDims >> 2) + 1;
	union xmmregister
	{
		__m128 m;
		DistanceValue f[4];
	} result;
#elif defined AVX
	int registersNumber = (trainSet.nrDims >> 3) + 1;
	union immregister
	{
		__m256 m;
		DistanceValue f[8];
	} result;
#endif
	//int currentTestSampleDistIndex = -1;
	//testSampleNNdists[0] = find1NNLimited(trainSet, testSample, testSampleDists, maximalRobustRanks[0], &currentTestSampleDistIndex);
	testSampleNNdists[0] = testSampleDists[0]; // first Sample in Sorted distances is 1NN
	centroids[0] = trainSet[testSampleNNdists[0].sampleIndex];
	swapDistances(testSampleDists, 0, maximalRobustRanks[k-1]-1);

	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
		// current centroid candidate
		// centroid does not have index or label, just dimensions
		Sample* currentCentroid = &centroids[centroidIndex]; 
		Sample closestCentroid;
		Sample* previousCentroid = &centroids[centroidIndex-1];

		int samIndexLimit = maximalRobustRanks[centroidIndex] - centroidIndex;
		int currentSamPos = -1;
		float divCentroidIndex = 1.0f / (centroidIndex+1);

#if defined SSE
		const __m128i centroidIndexISrc = _mm_set1_epi32(centroidIndex);
		const __m128 centroidIndexSrc = (__m128) _mm_cvtepi32_ps(centroidIndexISrc);
		const __m128 divCentroidIndexSrc = (__m128) _mm_set1_ps(divCentroidIndex);
#elif defined AVX
		const __m256i centroidIndexISrc = _mm256_set1_epi32(centroidIndex);
		const __m256 centroidIndexSrc = (__m256) _mm256_cvtepi32_ps(centroidIndexISrc);
		const __m256 divCentroidIndexSrc = (__m256) _mm256_set1_ps(divCentroidIndex);
#endif

		for (int samIndex = 0; samIndex < samIndexLimit; samIndex++) { // cache
			const Sample* trainSample = &trainSet[testSampleDists[samIndex].sampleIndex];

#if defined SSE	
			const __m128* currentCentroidSrc = (__m128*) currentCentroid->dims;
			const __m128* previousCentroidSrc = (__m128*) previousCentroid->dims;
			const __m128* trainSampleSrc = (__m128*) trainSample->dims;

			result.m = _mm_set1_ps(0x0);
			for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
				result.m = _mm_mul_ps(*previousCentroidSrc, centroidIndexSrc);
				result.m = _mm_add_ps(result.m, *trainSampleSrc);
				result.m = _mm_mul_ps(result.m, divCentroidIndexSrc);	

				_mm_store_ps(&(currentCentroid->dims[registerIndex*4]), result.m);	

				currentCentroidSrc++;
				previousCentroidSrc++;
				trainSampleSrc++;
			}
#elif defined AVX
			const __m256* currentCentroidSrc = (__m256*) currentCentroid->dims;
			const __m256* previousCentroidSrc = (__m256*) previousCentroid->dims;
			const __m256* trainSampleSrc = (__m256*) trainSample->dims;

			result.m = _mm256_set1_ps(0x0);
			for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
				result.m = _mm256_mul_ps(*previousCentroidSrc, centroidIndexSrc);
				result.m = _mm256_add_ps(result.m, *trainSampleSrc);
				result.m = _mm256_mul_ps(result.m, divCentroidIndexSrc);

				_mm256_store_ps(&(currentCentroid->dims[registerIndex*8]), result.m);

				currentCentroidSrc++;
				previousCentroidSrc++;
				trainSampleSrc++;
			}
#else
			for (int dimIndex = 0; dimIndex < trainSetNrDims; dimIndex++) {
				currentCentroid->dims[dimIndex] = ((previousCentroid->dims[dimIndex] * centroidIndex) + trainSample->dims[dimIndex]) * divCentroidIndex;
			}
#endif

#if defined MANHATTAN_DIST
			DistanceValue currentNCNdistVal = countManhattanDistance(*currentCentroid, testSample, 0, trainSet.nrDims);
#else
			DistanceValue currentNCNdistVal = countEuclideanDistance(*currentCentroid, testSample, 0, trainSet.nrDims);
#endif

			if (currentNCNdistVal < testSampleNNdists[centroidIndex].distValue) {
				// update current centroid candidate
				closestCentroid = *currentCentroid; // centroids[centroidIndex]
				
				// update data structure that keeps track of the trainSample that give the current nearest centroid
				testSampleNNdists[centroidIndex] =
					Distance(testSampleDists[samIndex].sampleIndex, testSampleDists[samIndex].sampleLabel, currentNCNdistVal);

				// update current trainSample postion in trainSet (because it could have been changed)
				// for situation when a just swapped trainSample is chosen
				currentSamPos = samIndex;
			}
		}
		// finally assign closestCentroid to return array 
		centroids[centroidIndex] = closestCentroid;
		
		// move current trainSample that gives the current closest centroid to the end of used trainDists
		// so it is not taken into account anymore
		swapDistances(testSampleDists, currentSamPos, (maximalRobustRanks[k-1]-1) - centroidIndex);
	}
}

void LimitedV2_kNCN::findkNCNLimitedV2(const SampleSet& trainSet, const Sample& testSample) {
	findkNCNLimitedV2(trainSet, testSample,
		this->distances[testSample.index], this->nndists[testSample.index], this->k);
}

// method should be used just once to find the optimal mRobust
// result of this method will be used than later as hardcoded value
// this should not be a part of preprocessing process
// it takes too many iterations to find the result
void LimitedV2_kNCN::learnMRobustRanks(SampleSet& trainSet) {
	Distance** trainDists = new Distance*[trainSet.nrSamples];
	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) {
		trainDists[distIndex] = new Distance[trainSet.nrSamples];
		std::fill(trainDists[distIndex], trainDists[distIndex]+trainSet.nrSamples, Distance(-1,-1, FLT_MAX));
	}
	Distance** trainNNdists = new Distance*[trainSet.nrSamples];
	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) {
		trainNNdists[distIndex] = new Distance[k];
		std::fill(trainNNdists[distIndex], trainNNdists[distIndex]+k, Distance(-1,-1, FLT_MAX));
	}

	// count distances between training samples and
	// find the farthest of kNCN neighbors for each sample
	// last Sample - trainSet.nrSamples - is the leave-one-out sample
	
	countDistancesLeaveOneOut(trainSet, trainDists);

	for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
		findkNCNLearn(trainSet, trainSet[samIndex],
			trainDists[samIndex], trainNNdists[samIndex], k);
		//LOG4CXX_DEBUG(logger, "" << trainNNdists[samIndex][0].sampleIndex << " " << trainNNdists[samIndex][0].distValue
		//	<< " " << trainNNdists[samIndex][1].sampleIndex << " " << trainNNdists[samIndex][1].distValue
		//	<< " " << trainNNdists[samIndex][2].sampleIndex << " " << trainNNdists[samIndex][2].distValue
		//	<< " " << trainNNdists[samIndex][3].sampleIndex << " " << trainNNdists[samIndex][3].distValue
		//	<< " " << trainNNdists[samIndex][4].sampleIndex << " " << trainNNdists[samIndex][4].distValue
		//	);
	}

	// sort according to distance from sample
	// percentMRobust
	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) {
		std::qsort(trainDists[distIndex], trainSet.nrSamples, sizeof(Distance), compareDistances);
	}

	// find rank of this kNCN neighbour
	// which sample in sorted distances array is it)
	// keep track of the maximal one
	// in the end found mRank for all samples - it is 100% now 
	int** maximalKRanks = new int*[k];
	for (int kIndex = 0; kIndex < k; kIndex++) {
		maximalKRanks[kIndex] = new int[trainSet.nrSamples]();
	}
	int* maximalKRank = new int[k];
	std::fill(maximalKRank, maximalKRank + k, -1);
	maximalKRank[0] = 0; // for k=0 it is always the 1NN

	for (int kIndex = 1; kIndex < k; kIndex++) {
		for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) {
				if (trainDists[samIndex][distIndex].sampleIndex == trainNNdists[samIndex][kIndex].sampleIndex) {	
					maximalKRanks[kIndex][samIndex] = distIndex; 
					if (distIndex > maximalKRank[kIndex]) {
						maximalKRank[kIndex] = distIndex;
						break;
					}
				}	
			}
		}
	}
	
	// averageMaximalRank = 29.
	//float averageMaximalRank = 0.0;
	//for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
	//	averageMaximalRank += maximalRanks[samIndex];
	//}
	//averageMaximalRank = averageMaximalRank/(trainSet.nrSamples - 1);
	//delete[] maximalRanks;

	// find percentSampleMaximal as (maximalRank/(nrTrainSamples-1) = 1330
	// maximalRobustRank = 113
	int percentSampleMaximal = (int)((percentMaxRobustRank * trainSet.nrSamples)/100.0);

	// look for mRobustRank decreasing m as long as there are more than 95% of samples that have 
	// their kNCN with rank smaller or equal than mRobust
	// this way we find minimal m (mRobust) for at least 95% of trainSamples
	for (int kIndex = 0; kIndex < k; kIndex++) {
		for (int rankIndex = maximalKRank[kIndex]; rankIndex >= 0; rankIndex--) {
			int countSamples = 0;
			for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
				if (maximalKRanks[kIndex][samIndex] < rankIndex) {
					countSamples++;
				}
			}

			if (countSamples < percentSampleMaximal) { 
				maximalRobustRanks[kIndex] = rankIndex;
				break;
			}
		}
	}
	//sampleSetFraction = maximalRobustRank/(trainSet.nrSamples-1);

	for (int rankIndex = 0; rankIndex < k; rankIndex++) {
		delete maximalKRanks[rankIndex];
	}
	delete[] maximalKRanks;

	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) {
		delete trainNNdists[distIndex];
	}
	delete[] trainNNdists;
	for (int distIndex = 0; distIndex < trainSet.nrSamples; distIndex++) { 
		delete trainDists[distIndex];
	}
	delete[] trainDists;
}

int LimitedV2_kNCN::assignLabel(const int testSampleIndex) {
	return Classifier::assignLabel(this->nndists[testSampleIndex], this->k);
}