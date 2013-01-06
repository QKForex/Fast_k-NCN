#include "util.h"

namespace Common {

	//TODO Get rid of remainder_table, some modulo hack needed

	SampleDim* allocateSampleDimsMemory(int nrDims, char* file, unsigned int line)
	{
		size_t size = (nrDims + remainder_table[nrDims % 4]) * sizeof(SampleDim);
		#ifdef SSE
		SampleDim* sampleDimsPtr = (SampleDim*) _aligned_malloc(size, 16);

		//TODO OutOfMemoryException(size)
		if (sampleDimsPtr == NULL) {
			throw "Not enough memory for sampleDims\n";
		}

		memset(sampleDimsPtr, 0, size);

		//TODO considered unsafe to use memset; intialize with fill() or look for SIMD instruction
		// fill does not work yet
		//fill(sampleDimsPtr, sampleDimsPtr+size, (SampleDim)0.0);

		#else
		 // initialized with zero
		SampleDim* sampleDimsPtr = new SampleDim[nrDims + remainder_table[nrDims % 4]]();

		//TODO OutOfMemoryException(size)
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

	void freeSampleDimsMemory(SampleDim* sampleDims, char* file, unsigned int line)
	{

		if (sampleDims != NULL)
		{
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

	void copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst)
	{
		for (int i = 0; i < nrDims; i++)
			dst[i] = src[i];
	}

	void standardize(SampleSet* train, SampleSet* test)
	{
		const int trainSize = train->getNrSamples();
		const int testSize = test->getNrSamples();
		const int nrDims = train->getNrDims();

		float* meanArray = NULL;
		float* sdevArray = NULL;

		try
		{
			meanArray = new float[nrDims];

			// Arithmetic mean
			for (int dimIndex = 0; dimIndex < nrDims; dimIndex++)
			{
				float sum = 0;
				for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++)
				{
					sum += (*train)[sampleIndex][dimIndex];
				}

				meanArray[dimIndex] = (float) sum / nrDims;
			}

			sdevArray = new float[nrDims];

			// Standard deviation
			for (int dimIndex = 0; dimIndex < nrDims; dimIndex++)
			{
				float sum = 0;
				for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++)
				{
					sum += ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex])
						* ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex]);
				}

				sdevArray[dimIndex] = sqrt((float) sum / nrDims);
			}

			// Trainset standardization
			for (int dimIndex = 0; dimIndex < nrDims; dimIndex++)
			{
				for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++)
				{
					if (sdevArray[dimIndex] != 0)
						(*train)[sampleIndex][dimIndex] = ((*train)[sampleIndex][dimIndex]
					- meanArray[dimIndex]) / sdevArray[dimIndex];
					else
						(*train)[sampleIndex][dimIndex] -= meanArray[dimIndex];
				}

			}

			// Testset standardization
			for (int dimIndex = 0; dimIndex < nrDims; dimIndex++)
			{
				for (int sampleIndex = 0; sampleIndex < testSize; sampleIndex++)
				{
					if (sdevArray[dimIndex] != 0)
						(*test)[sampleIndex][dimIndex] = ((*test)[sampleIndex][dimIndex]
					- meanArray[dimIndex]) / sdevArray[dimIndex];
					else
						(*test)[sampleIndex][dimIndex] -= meanArray[dimIndex];
				}

			}

			delete[] meanArray;
			delete[] sdevArray;
		}
		catch (bad_alloc &ba)
		{
			cout << ba.what() << endl;
			if (meanArray)
				delete[] meanArray;
			if (sdevArray)
				delete[] sdevArray;
		}
	}

}
