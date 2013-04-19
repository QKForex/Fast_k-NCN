#include "MemoryAllocation.h"

namespace Utility {

	//TODO: Get rid of remainder_table, some modulo hack needed

	//TODO: check and use Agner Fog alloc functions

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line) {
#ifdef SSE
		size_t size = (nrDims + REMAINDER_TABLE_SSE[nrDims % 4]) * sizeof(SampleDim);
		//size_t size = nrDims  * sizeof(SampleDim);
		SampleDim* sampleDimsPtr = (SampleDim*) _aligned_malloc(size, 16); // 16-byte aligned for 128-bit access
		//TODO: OutOfMemoryException(size)
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}
		memset(sampleDimsPtr, 0, size);
		//TODO: considered unsafe to use memset; intialize with fill() or look for SIMD instruction
		// fill does not work yet
		//fill(sampleDimsPtr, sampleDimsPtr+size, (SampleDim)0.0);
#elif defined AVX
		size_t size = (nrDims + REMAINDER_TABLE_AVX[nrDims % 8]) * sizeof(SampleDim);
		SampleDim* sampleDimsPtr = (SampleDim*) _aligned_malloc(size, 32); // 32-byte aligned for 256-bit access
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}
		memset(sampleDimsPtr, 0, size);
#else
		// initialized with zero
		SampleDim* sampleDimsPtr = new SampleDim[nrDims]();

		//TODO: OutOfMemoryException(size)
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}		
#endif

#ifdef LOGGING
		cerr << "Allocated " << size << " byte(s) at address " << sampleDimsPtr 
			<< " in " << file << ":" << line << endl;

#endif
		return sampleDimsPtr;
	}

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line) {
		if (sampleDims != NULL) {			
#if defined SSE || defined AVX
			_aligned_free(sampleDims);			
#else
			delete[] sampleDims;			
#endif
		
#ifdef LOGGING
			cerr << "Freed memory at address " << sampleDims
				<< " in " << file << ":" << line << endl;
#endif
			sampleDims = NULL;
		}
	}

}
