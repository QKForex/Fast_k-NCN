#include "MemoryAllocation.h"

namespace Utility {

	//TODO: Get rid of remainder_table, some modulo hack needed

	//TODO: check and use Agner Fog alloc functions

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line) {
		size_t size = (nrDims + REMAINDER_TABLE[nrDims % 4]) * sizeof(SampleDim);
		
#ifdef SSE
		SampleDim* sampleDimsPtr = (SampleDim*) _aligned_malloc(size, 16);
		//TODO: OutOfMemoryException(size)
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}
		memset(sampleDimsPtr, 0, size);
		//TODO: considered unsafe to use memset; intialize with fill() or look for SIMD instruction
		// fill does not work yet
		//fill(sampleDimsPtr, sampleDimsPtr+size, (SampleDim)0.0);
#else
		// initialized with zero
		SampleDim* sampleDimsPtr = new SampleDim[nrDims + remainder_table[nrDims % 4]]();

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
#ifdef SSE
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
