#pragma once	

//#include <algorithm>
#include <iostream>

// SSE intrinsics
#include <xmmintrin.h>
#include <emmintrin.h>

// AVX intrinsics
#include "immintrin.h"

#include "Config.h"

namespace Utility {

#if (defined SSE || defined AVX)
	const union ieee754_qnan {
		const SampleDim f;
		struct
		{
			const unsigned int mantissa:23, exp:8, sign:1;
		};

		ieee754_qnan() : f(0.0), mantissa(0x7fffff), exp(0xff), sign(0x0) {}
	} absmask;
#endif

#ifdef SSE
	static const __m128 abs4maskSSE = _mm_set1_ps(absmask.f);
	const static int REMAINDER_TABLE_SSE[] = { 0, 3, 2, 1 };
#elif defined AVX
	static const __m256 abs4maskAVX = _mm256_set1_ps(absmask.f);
	const static int REMAINDER_TABLE_AVX[] = { 0, 7, 6, 5, 4, 3, 2, 1 };
#endif

	//TODO: int (4-bytes) and double (8-bytes) SSE data vector (?)

	//TODO: remove remainder_table
	// try using alignas/alignof

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line);

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line);

}