#include "DistanceCalculation.h"

namespace Common {

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
			tmp = _mm_and_ps(abs4mask, tmp);
			result.m = _mm_add_ps(tmp, result.m);
			pSrc1++;
			pSrc2++;
		}
		return result.f[0] + result.f[1] + result.f[2] + result.f[3];
	}

	DistanceValue countEuclideanDistance(const Sample& train, const Sample& test,
		const int nrDims) {
		DistanceValue result = 0;
		
		for (int i = 0; i < nrDims; i++) {
			result += (test[i] - train[i]) * (test[i] - train[i]);
		}
		return result; // dropped sqrt for performance reasons (real distance not required)
	}

	//TODO: maybe should be const and array should not changed anywhere else in the code
	Distance* countDistances(const SampleSet& trainSet, const Sample& testSample) {
		Distance* distances = new Distance[trainSet.nrSamples];
		Distance d;
		int samIndex;
		//TODO: introduce multithreading
		//#pragma omp parallel for default(none) private(samIndex, d) shared(distances, trainSet, testSample) 
		for (samIndex = 0; samIndex < trainSet.nrSamples; samIndex++) {
			d.sampleIndex = samIndex;
#ifdef MANHATTAN_DIST
			d.distValue = countManhattanDistance(trainSet[samIndex], testSample, trainSet.nrDims);
#elif EUCLIDEAN_DIST
			d.distValue = countEuclideanDistance(trainSet[samIndex], testSample, trainSet.nrDims);
#endif
			d.sampleLabel = trainSet[samIndex].label;
			distances[samIndex] = d;
		}
		return distances;
	}

}