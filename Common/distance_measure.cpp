#include "distance_measure.h"

namespace Common {

	float countManhattanDistance(const Sample& train, const Sample& test, int nrDims)
	{
		int registersNumber = nrDims >> 2;

		//TODO needs refactoring
		const __m128* pSrc1 = (__m128*) train.getSampleDims();
		const __m128* pSrc2 = (__m128*) test.getSampleDims();

		__m128 tmp;
		union xmmregister
		{
			__m128 m;
			float f[4];
		} result;

		result.m = _mm_set1_ps(0x0);

		for (int registerIndex = 0; registerIndex < registersNumber; registerIndex++)
		{
			tmp = _mm_sub_ps(*pSrc2, *pSrc1);

			tmp = _mm_and_ps(abs4mask, tmp);

			result.m = _mm_add_ps(tmp, result.m);

			pSrc1++;
			pSrc2++;
		}

		return result.f[0] + result.f[1] + result.f[2] + result.f[3];
	}

	float countEuclideanDistance(const Sample& train, const Sample& test,
		const int nrDims)
	{
		float result = 0;

		for (int i = 0; i < nrDims; i++)
		{
			result += (test[i] - train[i]) * (test[i] - train[i]);
		}

		return result; // dropped sqrt for performance reasons (real distance not required)
	}

}