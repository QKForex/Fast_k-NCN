#include "DistanceCalculation.h"

namespace Common {

#ifdef SSE
	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int nrDims) {
		int registersNumber = (nrDims >> 2) + 1;
		//TODO: needs refactoring
		const __m128* pSrc1 = (__m128*) train.dims;
		const __m128* pSrc2 = (__m128*) test.dims;
		__m128 tmp;
		union xmmregister
		{
			__m128 m;
			DistanceValue f[4];
		} result;

		result.m = _mm_set1_ps(0x0);
		for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
			tmp = _mm_sub_ps(*pSrc2, *pSrc1);
			tmp = _mm_and_ps(abs4maskSSE, tmp);
			result.m = _mm_add_ps(tmp, result.m);
			pSrc1++;
			pSrc2++;
		}
		return result.f[0] + result.f[1] + result.f[2] + result.f[3];
	}
#elif defined AVX
	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int nrDims) {
		int registersNumber = (nrDims >> 3) + 1;
		//TODO: needs refactoring
		const __m256* pSrc1 = (__m256*) train.dims;
		const __m256* pSrc2 = (__m256*) test.dims;
		__m256 tmp;
		union immregister
		{
			__m256 m;
			DistanceValue f[8];
		} result;

		result.m = _mm256_set1_ps(0x0);
		for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++) {
			tmp = _mm256_sub_ps(*pSrc2, *pSrc1);
			tmp = _mm256_and_ps(abs4maskAVX, tmp);
			result.m = _mm256_add_ps(tmp, result.m);
			pSrc1++;
			pSrc2++;
		}
		return result.f[0] + result.f[1] + result.f[2] + result.f[3]
			+ result.f[4] + result.f[5] + result.f[6] + result.f[7];
	}
#else
	float countManhattanDistance(const Sample& train, const Sample& test, const int nrDims) {
		float result = 0;

		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
			result += fabs(test[dimIndex] - train[dimIndex]); // fabs performance bottleneck
		}

		return result;
	}
#endif
	DistanceValue countEuclideanDistance(const Sample& train, const Sample& test,
		const int nrDims) {
		DistanceValue result = 0;
		
		for (int i = 0; i < nrDims; i++) {
			result += (test[i] - train[i]) * (test[i] - train[i]);
		}
		return result; // dropped sqrt for performance reasons (real distance not required)
	}

	//TODO: maybe should be const and array should not changed anywhere else in the code
	void countDistances(const SampleSet& trainSet, const Sample& testSample, Distance* distances) {
		int samIndex;
		//TODO: introduce multithreading
		//#pragma omp parallel for default(none) private(samIndex, d) shared(distances, trainSet, testSample) 
		for (samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			distances[samIndex].sampleIndex = samIndex;
			distances[samIndex].sampleLabel = trainSet[samIndex].label;
#ifdef MANHATTAN_DIST
			distances[samIndex].distValue = countManhattanDistance(trainSet[samIndex], testSample, trainSet.nrDims);
#elif EUCLIDEAN_DIST
			distances[samIndex].distValue = countEuclideanDistance(trainSet[samIndex], testSample, trainSet.nrDims);
#endif
		}
	}

}