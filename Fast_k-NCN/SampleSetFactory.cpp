#include "SampleSetFactory.h"

namespace Common {
	//TODO: catch exceptions: bad_alloc on new
	//TODO: should not use assignment operator, redundant copying
	// int nrLoadSamples = 0 default: all samples
	SampleSet SampleSetFactory::createSampleSet(std::string inputFilename, const int nrLoadSamples, const int nrLoadDims) {
		std::ifstream inputFile;
		inputFile.open(inputFilename); // maybe check once again

		SampleSet set;
		inputFile >> set.nrClasses;
		inputFile >> set.nrDims;
		inputFile >> set.nrSamples;

		int nrOriginalDims = set.nrDims;

		if (nrLoadSamples != 0 && nrLoadSamples < set.nrSamples) {
			set.nrSamples = nrLoadSamples;
		}
		if (nrLoadDims != 0 && nrLoadDims < set.nrDims) {
			set.nrDims = nrLoadDims;
		}
		if (set.samples == NULL) {
			set.samples = new Sample[set.nrSamples];
		}
		for (int samIndex = 0; samIndex < set.nrSamples; samIndex++) {
			set[samIndex] = createSample(inputFile, samIndex, set.nrDims, nrOriginalDims); 
		}

		//for (Sample sam : set) {
		//	sam = createSample(inputFile, samIndex, set.nrDims, nrOriginalDims); 
		//}

		inputFile.close();
		return set;
	}

	Sample SampleSetFactory::createSample(std::ifstream& inputFile, 
		const int samIndex, const int nrSamDims, const int nrOriginalDims) {
		int samLabel;
		inputFile >> samLabel;
		Sample sample(samIndex, samLabel, nrSamDims);
		
		for (int dimIndex = 0; dimIndex < sample.nrDims; dimIndex++) {
			inputFile >> sample.dims[dimIndex];
		}

		float dummySampleDim; // hack to ignore the rest of dims from stream
		for (int dimIndex = sample.nrDims; dimIndex < nrOriginalDims; dimIndex++) {
			inputFile >> dummySampleDim;
		}

		return sample;
	}

}
