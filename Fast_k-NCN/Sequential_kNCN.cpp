#include "Sequential_kNCN.h"

LoggerPtr Sequential_kNCN::logger(Logger::getLogger("kNCNLogger"));

Sequential_kNCN::Sequential_kNCN() : Classifier(), centroids(SampleSet(-1, -1, k)) {}

Sequential_kNCN::Sequential_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples, 
								 const int nrClasses, const int nrDims)
	: Classifier(k, nrTrainSamples, nrTestSamples), centroids(SampleSet(nrClasses, nrDims, k)) {
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

Sequential_kNCN::~Sequential_kNCN() {
	if (results) { delete[] results; }
	if (nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
	if (distances) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
		delete[] distances;
	}
}

void Sequential_kNCN::preprocess(const SampleSet& trainSet, const SampleSet& testSet) {}

//
//	Perform classification
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
void Sequential_kNCN::classify(const SampleSet& trainSet, const SampleSet& testSet) {	  
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		countDistances(trainSet, testSet[samIndex], this->distances[samIndex]);
	}
	
	for (int samIndex = 0; samIndex < nrTestSamples; samIndex++) {
		results[samIndex] = classifySample(trainSet, testSet[samIndex]);
	}
}

//
//	Wrapper for find1NN that uses distances attribute instead of specified distances
//
const Distance Sequential_kNCN::find1NN(const SampleSet& trainSet, const Sample& testSample) {
	return Classifier::find1NN(trainSet, testSample, this->distances[testSample.index]);
}

//
//	Perform classification
//	does not change testSampleDists
//	preprocess should be run before to fill distances
//
//	Input:	trainSet, testSet
//	Output:	vector of assigned labels
//
int Sequential_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample,
	 Distance* testSampleDists, Distance* testSampleNNdists,  const int k) {	 
	if (k == 1) {
		return Classifier::find1NN(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCN(trainSet, testSample, testSampleDists, testSampleNNdists, k);
		//LOG4CXX_DEBUG(logger, "" << testSampleNNdists[0].sampleIndex << " " << testSampleNNdists[0].distValue
		//	<< " " << testSampleNNdists[1].sampleIndex << " " << testSampleNNdists[1].distValue
		//	<< " " << testSampleNNdists[2].sampleIndex << " " << testSampleNNdists[2].distValue
		//	<< " " << testSampleNNdists[3].sampleIndex << " " << testSampleNNdists[3].distValue
		//	<< " " << testSampleNNdists[4].sampleIndex << " " << testSampleNNdists[4].distValue
		//	);
		return assignLabel(testSample.index);
	}
}

int Sequential_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, this->distances[testSample.index],
		this->nndists[testSample.index], this->k);
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
void Sequential_kNCN::findkNCN(const SampleSet& trainSet, const Sample& testSample,
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
	swapDistances(testSampleDists, testSampleNNdists[0].sampleIndex, trainSet.nrSamples - 1);

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

#if defined MANHATTAN_DIST
			DistanceValue currentNCNdistVal = countManhattanDistance(*currentCentroid, testSample, 0, trainSet.nrDims);
#else
			DistanceValue currentNCNdistVal = countEuclideanDistance(*currentCentroid, testSample, 0, trainSet.nrDims);
#endif

			if (currentNCNdistVal < testSampleNNdists[centroidIndex].distValue) {
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
		swapDistances(testSampleDists, currentSamPos, trainSet.nrSamples-1 - centroidIndex);
	}
}

void Sequential_kNCN::findkNCN(const SampleSet& trainSet, const Sample& testSample) {
	return findkNCN(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}

int Sequential_kNCN::assignLabel(const int testSampleIndex) {
	return Classifier::assignLabel(this->nndists[testSampleIndex], this->k);
}
