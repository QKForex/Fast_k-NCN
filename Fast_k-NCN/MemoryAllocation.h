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

	const union ieee754_qnan {
		const SampleDim f;
		struct
		{
			const unsigned int mantissa:23, exp:8, sign:1;
		};

		ieee754_qnan() : f(0.0), mantissa(0x7fffff), exp(0xff), sign(0x0) {}
	} absmask;

	static const __m128 abs4mask = _mm_load1_ps(&absmask.f);

	//TODO int (4-bytes) and double (8-bytes) SSE data vector (?)

	//TODO AVX 

	//TODO remove remainder_table
	// try using alignas/alignof

	//TODO do it!
	const static int REMAINDER_TABLE[] = { 0, 3, 2, 1 };

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line);

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line);

}