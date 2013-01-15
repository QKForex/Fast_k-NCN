#pragma once
#ifndef COMMON_SSE_UTIL_H_
#define COMMON_SSE_UTIL_H_

#include <xmmintrin.h>
#include <emmintrin.h>

namespace Common {

	const union ieee754_qnan {
		const SampleDim f;
		struct
		{
			const unsigned int mantissa:23, exp:8, sign:1;
		};

		ieee754_qnan() : f(0.0), mantissa(0x7fffff), exp(0xff), sign(0x0) {}
	} absmask;

	static const __m128 abs4mask = _mm_load1_ps(&absmask.f);

	//TODO int (4-bytes) and double (8-bytes) SSE data vector

	//TODO AVX 

}

#endif /* COMMON_SSE_UTIL_H_ */