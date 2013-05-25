#include "CacheEfficient_kNCN.h"

LoggerPtr CacheEfficient_kNCN::logger(Logger::getLogger("Cache_kNCNLogger"));

CacheEfficient_kNCN::CacheEfficient_kNCN() : Classifier(), centroids(SampleSet(-1, -1, k)) {}

CacheEfficient_kNCN::CacheEfficient_kNCN(const int k, const int nrTrainSamples,
	const int nrTestSamples, const int nrClasses, const int nrDims, const int nrSamplesInBlock)
	: Classifier(k, nrTrainSamples, nrTestSamples), centroids(SampleSet(nrClasses, nrDims, k)) {

	if (nrSamplesInBlock == -1 || nrSamplesInBlock > nrTrainSamples || nrSamplesInBlock > nrTestSamples) {
		this->nrSamplesInTrainBlock = nrTrainSamples;
		this->nrSamplesInTestBlock = nrTestSamples;
	} else {
		this->nrSamplesInTrainBlock = nrSamplesInBlock;
		this->nrSamplesInTestBlock = nrSamplesInBlock;
	}

	nrTrainSetBlocks = (int) ceil((float)nrTrainSamples/nrSamplesInTrainBlock);
	nrTestSetBlocks = (int) ceil((float)nrTestSamples/nrSamplesInTestBlock);

	firstRemainingTrainSampleIndex = nrTrainSetBlocks != 1 ? (nrTrainSetBlocks - 1)*nrSamplesInTrainBlock : 0;
	nrRemainingTrainSamples = nrTrainSamples - firstRemainingTrainSampleIndex;

	firstRemainingTestSampleIndex = nrTestSetBlocks != 1 ? (nrTestSetBlocks - 1)*nrSamplesInTestBlock : 0;
	nrRemainingTestSamples = nrTestSamples - firstRemainingTestSampleIndex;

	blockDistances = new Distance***[nrTestSetBlocks];
	for (int testDistIndex = 0; testDistIndex < nrTestSetBlocks; testDistIndex++) {
		blockDistances[testDistIndex] = new Distance**[nrTrainSetBlocks];
		for (int trainDistIndex = 0; trainDistIndex < nrTrainSetBlocks; trainDistIndex++) {
			blockDistances[testDistIndex][trainDistIndex] = new Distance*[nrSamplesInTestBlock];
			for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) {
				blockDistances[testDistIndex][trainDistIndex][distIndex] = new Distance[nrSamplesInTrainBlock];
				std::fill(blockDistances[testDistIndex][trainDistIndex][distIndex],
					blockDistances[testDistIndex][trainDistIndex][distIndex]+nrSamplesInTrainBlock,
					Distance());
			}		
		}
	}

	blockNCNDists = new NCNDistance***[nrTestSetBlocks];
	for (int testDistIndex = 0; testDistIndex < nrTestSetBlocks; testDistIndex++) {
		blockNCNDists[testDistIndex] = new NCNDistance**[nrTrainSetBlocks];
		for (int trainDistIndex = 0; trainDistIndex < nrTrainSetBlocks; trainDistIndex++) {
			blockNCNDists[testDistIndex][trainDistIndex] = new NCNDistance*[nrSamplesInTestBlock];
			for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) {
				blockNCNDists[testDistIndex][trainDistIndex][distIndex] = new NCNDistance[k];
				std::fill(blockNCNDists[testDistIndex][trainDistIndex][distIndex],
					blockNCNDists[testDistIndex][trainDistIndex][distIndex]+k,
					NCNDistance());
			}		
		}
	}

	closestNCNDists = new NCNDistance*[nrSamplesInTestBlock];
	for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) {
		closestNCNDists[distIndex] = new NCNDistance[nrTrainSetBlocks*k];
		std::fill(closestNCNDists[distIndex], closestNCNDists[distIndex]+nrTrainSetBlocks*k, NCNDistance());
	}

	for (int centroidIndex = 0; centroidIndex < k; centroidIndex++) {
		centroids[centroidIndex].nrDims = nrDims;
		centroids[centroidIndex].dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
	}
}

CacheEfficient_kNCN::~CacheEfficient_kNCN() {
	if (closestNCNDists) {
		for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) { delete closestNCNDists[distIndex]; }
		delete[] closestNCNDists;
	}	
	
	if (blockNCNDists) {
		for (int testDistIndex = 0; testDistIndex < nrTestSetBlocks; testDistIndex++) {
			for (int trainDistIndex = 0; trainDistIndex < nrTrainSetBlocks; trainDistIndex++) {
				for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) {
					delete blockNCNDists[testDistIndex][trainDistIndex][distIndex];
				}
				delete[] blockNCNDists[testDistIndex][trainDistIndex];
			}
			delete[] blockNCNDists[testDistIndex];
		}
		delete[] blockNCNDists;
	}

	if (blockDistances) {
		for (int testDistIndex = 0; testDistIndex < nrTestSetBlocks; testDistIndex++) {
			for (int trainDistIndex = 0; trainDistIndex < nrTrainSetBlocks; trainDistIndex++) {
				for (int distIndex = 0; distIndex < nrSamplesInTestBlock; distIndex++) {
					delete blockDistances[testDistIndex][trainDistIndex][distIndex];
				}
				delete[] blockDistances[testDistIndex][trainDistIndex];
			}
			delete[] blockDistances[testDistIndex];
		}
		delete[] blockDistances;
	}
	
	if (results) { delete[] results; }
}

void CacheEfficient_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
void CacheEfficient_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int testBlockIndex = 0; testBlockIndex < nrTestSetBlocks - 1; testBlockIndex++) {
		SampleSet testSetBlock(testSet.nrClasses, testSet.nrDims, nrSamplesInTestBlock);
		for (int samIndex = 0; samIndex < nrSamplesInTestBlock; samIndex++)	{
			testSetBlock[samIndex] = testSet[testBlockIndex*nrSamplesInTestBlock + samIndex]; 
		}
		for (int trainBlockIndex = 0; trainBlockIndex < nrTrainSetBlocks - 1; trainBlockIndex++) {
			SampleSet trainSetBlock(trainSet.nrClasses, trainSet.nrDims, nrSamplesInTrainBlock);
			for (int samIndex = 0; samIndex < nrSamplesInTrainBlock; samIndex++) {
				trainSetBlock[samIndex] = trainSet[trainBlockIndex*nrSamplesInTrainBlock + samIndex]; 
			}
			classifyBlock(trainSetBlock, testSetBlock, this->blockDistances[testBlockIndex][trainBlockIndex], 
				this->blockNCNDists[testBlockIndex][trainBlockIndex], trainBlockIndex*nrSamplesInTrainBlock, testBlockIndex*nrSamplesInTestBlock);
		}
		// last train block
		SampleSet trainSetBlock(trainSet.nrClasses, trainSet.nrDims, nrRemainingTrainSamples);
		for (int samIndex = 0; samIndex < trainSetBlock.nrSamples; samIndex++) {
			trainSetBlock[samIndex] = trainSet[firstRemainingTrainSampleIndex + samIndex]; 
		}
		classifyBlock(trainSetBlock, testSetBlock, this->blockDistances[testBlockIndex][nrTrainSetBlocks - 1], 
			this->blockNCNDists[testBlockIndex][nrTrainSetBlocks - 1], firstRemainingTrainSampleIndex, testBlockIndex*nrSamplesInTestBlock);
		
		// store NCNs for every testSample from current testSetBlock from all trainBlocks for every k
		for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
			for (int trainBlockIndex = 0; trainBlockIndex < nrTrainSetBlocks; trainBlockIndex++) {
				for (int kIndex = 0; kIndex < k; kIndex++)	{
					closestNCNDists[samIndex][trainBlockIndex*k + kIndex] = blockNCNDists[testBlockIndex][trainBlockIndex][samIndex][kIndex];
				}
			}
		}
		for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
			// sort NCNs on basis of distance from sample
			qsort(closestNCNDists[samIndex], nrTrainSetBlocks*k, sizeof(NCNDistance), compareNCNDistances);
			// assign label on basis of closestNCNDists
			results[testBlockIndex*nrSamplesInTestBlock + samIndex] = assignLabel(closestNCNDists[samIndex], k);
			//LOG4CXX_DEBUG(logger, "" << closestNCNDists[samIndex][0].sampleIndex << " " << closestNCNDists[samIndex][0].distValue
			//	<< " " << closestNCNDists[samIndex][1].sampleIndex << " " << closestNCNDists[samIndex][1].distValue
			//	<< " " << closestNCNDists[samIndex][2].sampleIndex << " " << closestNCNDists[samIndex][2].distValue
			//	<< " " << closestNCNDists[samIndex][3].sampleIndex << " " << closestNCNDists[samIndex][3].distValue
			//	<< " " << closestNCNDists[samIndex][4].sampleIndex << " " << closestNCNDists[samIndex][4].distValue
			//	);
		}
	}

	// last test block
	SampleSet testSetBlock(testSet.nrClasses, testSet.nrDims, nrRemainingTestSamples);
	for (int samIndex = 0; samIndex < nrRemainingTestSamples; samIndex++) {
		testSetBlock[samIndex] = testSet[firstRemainingTestSampleIndex + samIndex]; 
	}
	for (int trainBlockIndex = 0; trainBlockIndex < nrTrainSetBlocks - 1; trainBlockIndex++) {
		SampleSet trainSetBlock(trainSet.nrClasses, trainSet.nrDims, nrSamplesInTrainBlock);
		for (int samIndex = 0; samIndex < nrSamplesInTrainBlock; samIndex++) {
			trainSetBlock[samIndex] = trainSet[trainBlockIndex*nrSamplesInTrainBlock + samIndex]; 
		}
		classifyBlock(trainSetBlock, testSetBlock, this->blockDistances[nrTestSetBlocks - 1][trainBlockIndex], 
			this->blockNCNDists[nrTestSetBlocks - 1][trainBlockIndex], trainBlockIndex*nrSamplesInTrainBlock, firstRemainingTestSampleIndex);
	}
	// last train block for last test block
	SampleSet trainSetBlock(trainSet.nrClasses, trainSet.nrDims, nrRemainingTrainSamples);
	for (int samIndex = 0; samIndex < nrRemainingTrainSamples; samIndex++) {
		trainSetBlock[samIndex] = trainSet[firstRemainingTrainSampleIndex + samIndex]; 
	}
	classifyBlock(trainSetBlock, testSetBlock, this->blockDistances[nrTestSetBlocks - 1][nrTrainSetBlocks - 1], 
		this->blockNCNDists[nrTestSetBlocks - 1][nrTrainSetBlocks - 1], firstRemainingTrainSampleIndex, firstRemainingTestSampleIndex);

	// find the closest candidate for every testSample from current testSetBlock for kNCN from all trainBlocks for every k
	for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
		for (int trainBlockIndex = 0; trainBlockIndex < nrTrainSetBlocks; trainBlockIndex++) {
			for (int kIndex = 0; kIndex < k; kIndex++)	{
				closestNCNDists[samIndex][trainBlockIndex*k + kIndex] = blockNCNDists[nrTestSetBlocks - 1][trainBlockIndex][samIndex][kIndex];
			}
		}
	}
	for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
		// sort NCNs on basis of distance from sample
		qsort(closestNCNDists[samIndex], nrTrainSetBlocks*k, sizeof(NCNDistance), compareNCNDistances);
		// assign label on basis of closestNCNDists
		results[firstRemainingTestSampleIndex + samIndex] = assignLabel(closestNCNDists[samIndex], k);
		//LOG4CXX_DEBUG(logger, "" << closestNCNDists[samIndex][0].sampleIndex << " " << closestNCNDists[samIndex][0].distValue
		//	<< " " << closestNCNDists[samIndex][1].sampleIndex << " " << closestNCNDists[samIndex][1].distValue
		//	<< " " << closestNCNDists[samIndex][2].sampleIndex << " " << closestNCNDists[samIndex][2].distValue
		//	<< " " << closestNCNDists[samIndex][3].sampleIndex << " " << closestNCNDists[samIndex][3].distValue
		//	<< " " << closestNCNDists[samIndex][4].sampleIndex << " " << closestNCNDists[samIndex][4].distValue
		//	);
	}
}

void CacheEfficient_kNCN::classifyBlock(const SampleSet& trainSetBlock,
	const SampleSet& testSetBlock, Distance** distances, NCNDistance** ncndists, 
	const int trainBlockOffset, const int testBlockOffset) {
	
	for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
		countDistancesCache(trainSetBlock, testSetBlock[samIndex], distances[samIndex], trainBlockOffset);
	}

	if (k == 1) {
		for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
			//ncndists[samIndex][0] = Classifier::find1NN(trainSetBlock, testSetBlock[samIndex], distances[samIndex]);
		}
	} else {
		for (int samIndex = 0; samIndex < testSetBlock.nrSamples; samIndex++) {
			findkNCN(trainSetBlock, testSetBlock[samIndex], distances[samIndex], ncndists[samIndex], k, trainBlockOffset, testBlockOffset);
		}
	}
}

//
//	Perform classification
//	does not change testSampleDists
//	preprocess should be run before to fill distances
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
int CacheEfficient_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, Distance* testSampleNNdists,  const int k) {	 
	return -1;
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
//	we set the distance to FLT_MAX for chosen kNCN sample in distances once
//	this excludes first kNCN from being chosen again
//	this action will be repeated for the rest 
//	distances[nndists[0].sampleIndex].distValue = FLT_MAX;
//	check if given sample is not kNCN already
//
void CacheEfficient_kNCN::findkNCN(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists, NCNDistance* testSampleNCNdists, const int k, const int trainBlockOffset, const int testBlockOffset) {
	const int trainSetNrDims = trainSet.nrDims;
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

	Distance testSampleNNdist = Classifier::find1NN(trainSet, testSample, testSampleDists);
	testSampleNCNdists[0].sampleIndex = testSampleNNdist.sampleIndex;
	testSampleNCNdists[0].sampleLabel = testSampleNNdist.sampleLabel;
	testSampleNCNdists[0].distValue = testSampleNNdist.distValue;
	testSampleNCNdists[0].ncnDistValue = 0.0; //TODO: refactor

	centroids[0] = trainSet[testSampleNCNdists[0].sampleIndex - trainBlockOffset];
	swapDistances(testSampleDists, testSampleNCNdists[0].sampleIndex - trainBlockOffset, trainSet.nrSamples - 1);

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
			const Sample* trainSample = &trainSet[testSampleDists[samIndex].sampleIndex - trainBlockOffset];
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

			if (currentNCNdistVal < testSampleNCNdists[centroidIndex].ncnDistValue) {
				// update current centroid candidate
				closestCentroid = *currentCentroid;

				// update data structure that keeps track of the trainSample that give the current nearest centroid
				testSampleNCNdists[centroidIndex] =
					NCNDistance(testSampleDists[samIndex].sampleIndex, testSampleDists[samIndex].sampleLabel,
						testSampleDists[samIndex].distValue, currentNCNdistVal);

				// update current trainSample postion in trainSet (because it could have been changed)
				// for situation when a just swapped trainSample is chosen
				currentSamPos = samIndex;
			}
		}
		// finally assign closestCentroid to return array 
		centroids[centroidIndex] = closestCentroid;
		
		// move current trainSample that gives the current nearest centroid to the end of trainSet
		// so it is not taken into account anymore
		swapDistances(testSampleDists, currentSamPos, trainSet.nrSamples-1 - centroidIndex);
	}
}

int CacheEfficient_kNCN::assignLabel(const NCNDistance* testSampleNCNdists, const int k) {
	Distance* testSampleNNdists = new Distance[k];
	for (int distIndex = 0; distIndex < k; distIndex++) {
		testSampleNNdists[distIndex].sampleIndex = testSampleNCNdists[distIndex].sampleIndex;
		testSampleNNdists[distIndex].sampleLabel = testSampleNCNdists[distIndex].sampleLabel;
		testSampleNNdists[distIndex].distValue = testSampleNCNdists[distIndex].distValue;
	}

	return Classifier::assignLabel(testSampleNNdists, k);
}