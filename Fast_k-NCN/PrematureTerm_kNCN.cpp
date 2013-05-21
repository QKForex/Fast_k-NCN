#include "PrematureTerm_kNCN.h"

LoggerPtr PrematureTerm_kNCN::logger(Logger::getLogger("PT_kNCNLogger"));

PrematureTerm_kNCN::PrematureTerm_kNCN(): Classifier(), centroids(SampleSet(-1, -1, k)), threshold(-1) {} 

PrematureTerm_kNCN::PrematureTerm_kNCN(const int k, const int nrTrainSamples, const int nrTestSamples,
									   const int nrClasses, const int nrDims, const int threshold)
	: Classifier(k, nrTrainSamples, nrTestSamples), centroids(SampleSet(nrClasses, nrDims, k)) {
	if (threshold == -1) {
		this->threshold = nrDims; // no threshold set
	} else {
		this->threshold = threshold;
	}

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

	for (int centroidIndex = 0; centroidIndex < k; centroidIndex++) {
		centroids[centroidIndex].nrDims = nrDims;
		centroids[centroidIndex].dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
	}
}

PrematureTerm_kNCN::~PrematureTerm_kNCN() {
	if (nndists) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete nndists[distIndex]; }
		delete[] nndists;
	}
	if (distances) {
		for (int distIndex = 0; distIndex < nrTestSamples; distIndex++) { delete distances[distIndex]; }
		delete[] distances;
	}

	if (results) { delete[] results; }
}

// no preprocessing phase in this algorithm
// distances are counted in find1NN step of classification
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
		return find1NNPrematureTerm(trainSet, testSample, testSampleDists).sampleLabel;
	} else {
		findkNCNPrematureTerm(trainSet, testSample, testSampleDists, testSampleNNdists, k);
		//LOG4CXX_DEBUG(logger, "" << testSampleNNdists[0].sampleIndex << " " << testSampleNNdists[0].distValue
		//<< " " << testSampleNNdists[1].sampleIndex << " " << testSampleNNdists[1].distValue
		//<< " " << testSampleNNdists[2].sampleIndex << " " << testSampleNNdists[2].distValue
		//<< " " << testSampleNNdists[3].sampleIndex << " " << testSampleNNdists[3].distValue
		//<< " " << testSampleNNdists[4].sampleIndex << " " << testSampleNNdists[4].distValue
		//);
		return Classifier::assignLabel(testSampleNNdists, k);
	}
}

int PrematureTerm_kNCN::classifySample(const SampleSet& trainSet, const Sample& testSample) {
	return classifySample(trainSet, testSample, this->distances[testSample.index],
		this->nndists[testSample.index], this->k);
}

const Distance PrematureTerm_kNCN::find1NNPrematureTerm(const SampleSet& trainSet, const Sample& testSample,
	Distance* testSampleDists) {
	testSampleDists[0] = Distance(trainSet[0].index, trainSet[0].label, 
		countManhattanDistance(trainSet[0], testSample, 0, testSample.nrDims));
	Distance* nearestNeighbourDist = &testSampleDists[0];

	for (int distsIndex = 1; distsIndex < trainSet.nrSamples; distsIndex++) {
		testSampleDists[distsIndex].sampleIndex = trainSet[distsIndex].index;
		testSampleDists[distsIndex].sampleLabel = trainSet[distsIndex].label;
		testSampleDists[distsIndex].distValue = countManhattanDistance(trainSet[distsIndex], testSample, 0, this->threshold);
		if (testSampleDists[distsIndex].distValue < (*nearestNeighbourDist).distValue) {
			testSampleDists[distsIndex].distValue += countManhattanDistance(trainSet[distsIndex], testSample, this->threshold, testSample.nrDims);
			if (testSampleDists[distsIndex].distValue < (*nearestNeighbourDist).distValue) {
				nearestNeighbourDist = &testSampleDists[distsIndex];
			}
		}
	}

	return *nearestNeighbourDist;
}

void PrematureTerm_kNCN::findkNCNPrematureTerm(const SampleSet& trainSet, const Sample& testSample,
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

	testSampleNNdists[0] = find1NNPrematureTerm(trainSet, testSample, testSampleDists);
	centroids[0] = trainSet[testSampleNNdists[0].sampleIndex];
	swapDistances(testSampleDists, testSampleNNdists[0].sampleIndex, trainSet.nrSamples - 1);

	for (int centroidIndex = 1; centroidIndex < k; centroidIndex++) {
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
			DistanceValue currentNCNdistVal = countManhattanDistance(*currentCentroid, testSample, 0, this->threshold);
#else
			DistanceValue currentNCNdistVal = countEuclideanDistance(*currentCentroid, testSample, 0, this->threshold);
#endif

			if (currentNCNdistVal < testSampleNNdists[centroidIndex].distValue) {
				currentNCNdistVal += countManhattanDistance(*currentCentroid, testSample, this->threshold, testSample.nrDims);
				
				if (currentNCNdistVal < testSampleNNdists[centroidIndex].distValue) {
					closestCentroid = *currentCentroid;		
					testSampleNNdists[centroidIndex] = Distance(testSampleDists[samIndex].sampleIndex, testSampleDists[samIndex].sampleLabel, currentNCNdistVal);
					currentSamPos = samIndex;
				}
			}
		}
		
		centroids[centroidIndex] = closestCentroid;
		swapDistances(testSampleDists, currentSamPos, trainSet.nrSamples-1 - centroidIndex);
	}
}

void PrematureTerm_kNCN::findkNCNPrematureTerm(const SampleSet& trainSet, const Sample& testSample) {
	return findkNCNPrematureTerm(trainSet, testSample, this->distances[testSample.index], this->nndists[testSample.index], this->k);
}

int PrematureTerm_kNCN::assignLabel(const int testSampleIndex) {
	return Classifier::assignLabel(this->nndists[testSampleIndex], this->k);
}