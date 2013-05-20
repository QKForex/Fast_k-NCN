#include "DistanceCalculation.h"

namespace Common {

#if defined SSE
	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int firstDim, const int lastDim) {
		const int registersNumber = (((lastDim-1)-firstDim) >> 2) + 1;
		const __m128* trainDimsSrc = (__m128*) &train.dims[firstDim];
		const __m128* testDimsSrc = (__m128*) &test.dims[firstDim];
		__m128 tmp;
		union xmmregister {
			__m128 m;
			DistanceValue f[4];
		} result;

		result.m = _mm_set1_ps(0x0);
		for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
			tmp = _mm_sub_ps(*testDimsSrc, *trainDimsSrc);
			tmp = _mm_and_ps(abs4maskSSE, tmp);
			result.m = _mm_add_ps(tmp, result.m);
			trainDimsSrc++;
			testDimsSrc++;
		}

		result.m = _mm_hadd_ps(result.m, result.m);
		result.m = _mm_hadd_ps(result.m, result.m);

		return result.f[0];
	}
#elif defined AVX
	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int firstDim, const int lastDim) {
		const int registersNumber = (((lastDim-1)-firstDim) >> 3) + 1;
		const __m256* trainDimsSrc = (__m256*) &train.dims[firstDim];
		const __m256* testDimsSrc = (__m256*) &test.dims[firstDim];
		__m256 tmp;
		union immregister {
			__m256 m;
			DistanceValue f[8];
		} result;

		result.m = _mm256_set1_ps(0x0);
		for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
			tmp = _mm256_sub_ps(*testDimsSrc, *trainDimsSrc);
			tmp = _mm256_and_ps(abs4maskAVX, tmp);
			result.m = _mm256_add_ps(tmp, result.m);
			trainDimsSrc++;
			testDimsSrc++;
		}

		result.m = _mm256_hadd_ps(result.m, result.m);
		result.m = _mm256_hadd_ps(result.m, result.m);

		return result.f[0] + result.f[4];
	}
#else
	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int firstDim, const int lastDim) {
		float result = 0;

		for (int dimIndex = firstDim; dimIndex < lastDim; dimIndex++) {
			result += fabs(test[dimIndex] - train[dimIndex]); // fabs performance bottleneck
		}

		return result;
	}
#endif

	DistanceValue countEuclideanDistance(const Sample& train, const Sample& test, const int firstDim, const int lastDim) {
		DistanceValue result = 0;
		
		for (int i = firstDim; i < lastDim; i++) {
			result += (test[i] - train[i]) * (test[i] - train[i]);
		}

		return result; // dropped sqrt for performance reasons (real distance not required)
	}

	void countDistances(const SampleSet& trainSet, const Sample& testSample, Distance* distances) {
		for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			distances[samIndex].sampleIndex = samIndex;
			distances[samIndex].sampleLabel = trainSet[samIndex].label;
#ifdef MANHATTAN_DIST
			distances[samIndex].distValue = countManhattanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#elif EUCLIDEAN_DIST
			distances[samIndex].distValue = countEuclideanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#endif
		}
	}

	void countDistancesParallel(const SampleSet& trainSet, const Sample& testSample, Distance* distances) {
		int samIndex;
		#pragma omp parallel for default(shared) private(samIndex) 
		for (samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			distances[samIndex].sampleIndex = samIndex;
			distances[samIndex].sampleLabel = trainSet[samIndex].label;
#ifdef MANHATTAN_DIST
			distances[samIndex].distValue = countManhattanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#elif EUCLIDEAN_DIST
			distances[samIndex].distValue = countEuclideanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#endif
		}
	}
	
	void countDistancesCache(const SampleSet& trainSet, const Sample& testSample, Distance* distances, const int blockOffset) {
		for (int samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			distances[samIndex].sampleIndex = samIndex + blockOffset;
			distances[samIndex].sampleLabel = trainSet[samIndex].label;
#ifdef MANHATTAN_DIST
			distances[samIndex].distValue = countManhattanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#elif EUCLIDEAN_DIST
			distances[samIndex].distValue = countEuclideanDistance(trainSet[samIndex], testSample, 0, testSample.nrDims);
#endif
		}
	}

	void countDistancesLeaveOneOut(SampleSet& sampleSet, Distance** distances) {
		//int samIndex;
		//TODO: introduce multithreading
		//#pragma omp parallel for default(none) private(samIndex, d) shared(distances, sampleSet, testSample) 
		for (int testSamIndex = 0; testSamIndex < sampleSet.nrSamples - 1; testSamIndex++) {
			sampleSet.swapSamples(testSamIndex, sampleSet.nrSamples - 1);
			//swapDistances(distances[testSamIndex], testSamIndex, sampleSet.nrSamples - 1);
			for (int trainSamIndex = 0; trainSamIndex < sampleSet.nrSamples - 1; trainSamIndex++) {
				distances[testSamIndex][trainSamIndex].sampleIndex = sampleSet[trainSamIndex].index;
				distances[testSamIndex][trainSamIndex].sampleLabel = sampleSet[trainSamIndex].label;
#ifdef MANHATTAN_DIST
					distances[testSamIndex][trainSamIndex].distValue = 
							countManhattanDistance(sampleSet[trainSamIndex], sampleSet[sampleSet.nrSamples - 1], 0, sampleSet.nrDims);
#elif EUCLIDEAN_DIST
					distances[testSamIndex][trainSamIndex].distValue
						= countEuclideanDistance(sampleSet[trainSamIndex], sampleSet[sampleSet.nrSamples - 1], 0, sampleSet.nrDims);
#endif
			}	
			distances[testSamIndex][sampleSet.nrSamples - 1].sampleIndex = sampleSet[sampleSet.nrSamples - 1].index;
			distances[testSamIndex][sampleSet.nrSamples - 1].sampleLabel = sampleSet[sampleSet.nrSamples - 1].label;
			swapDistances(distances[testSamIndex], sampleSet.nrSamples - 1, testSamIndex);
			sampleSet.swapSamples(sampleSet.nrSamples - 1, testSamIndex);


			// do not set distValue, because it was already set with FLT_MAX
			// in reality the distance is 0, because it is the same sample
			// but it should not be taken into consideration
		}
		// no swapping for last testSample
		for (int trainSamIndex = 0; trainSamIndex < sampleSet.nrSamples-1; trainSamIndex++) {
			distances[sampleSet.nrSamples - 1][trainSamIndex].sampleIndex = sampleSet[trainSamIndex].index;
			distances[sampleSet.nrSamples - 1][trainSamIndex].sampleLabel = sampleSet[trainSamIndex].label;
#ifdef MANHATTAN_DIST
			distances[sampleSet.nrSamples - 1][trainSamIndex].distValue = 
					countManhattanDistance(sampleSet[trainSamIndex], sampleSet[sampleSet.nrSamples - 1], 0, sampleSet.nrDims);
#elif EUCLIDEAN_DIST
			distances[sampleSet.nrSamples - 1][trainSamIndex].distValue
				= countEuclideanDistance(sampleSet[trainSamIndex], sampleSet[sampleSet.nrSamples - 1], 0, sampleSet.nrDims);
#endif
		}
		distances[sampleSet.nrSamples - 1][sampleSet.nrSamples - 1].sampleIndex = sampleSet[sampleSet.nrSamples - 1].index;
		distances[sampleSet.nrSamples - 1][sampleSet.nrSamples - 1].sampleLabel = sampleSet[sampleSet.nrSamples - 1].label;

	}
}
