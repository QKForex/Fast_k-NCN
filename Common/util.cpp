#include "util.h"

namespace Common {

	//TODO Get rid of remainder_table, some modulo hack needed

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line)
	{
		//SampleDim* newSampleDims = new SampleDim[nrDims + remainder_table[nrDims % 4]];
		size_t size = (nrDims + remainder_table[nrDims % 4]) * sizeof(SampleDim);

		SampleDim* sampleDimsPtr = (SampleDim*) _aligned_malloc(size, 16);

		//TODO OutOfMemoryException(size)
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}

		memset(sampleDimsPtr, 0, size);

		#ifdef LOGGING
		cerr << "Allocated " << size << " byte(s) at address " << sampleDimsPtr 
			<< " in " << file << ":" << line << endl;
		#endif

		return sampleDimsPtr;
	}

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line)
	{

		if (sampleDims != NULL)
		{
			//delete[] sampleDims;
			_aligned_free(sampleDims);

			#ifdef LOGGING
			cerr << "Freed memory at address " << sampleDims
				<< " in " << file << ":" << line << endl;
			#endif

			sampleDims = NULL;
		}
	}

	void copySampleDims(SampleDim* src, int nrDims, SampleDim* dst)
	{
		for (int i = 0; i < nrDims; i++)
			dst[i] = src[i];
	}

}
