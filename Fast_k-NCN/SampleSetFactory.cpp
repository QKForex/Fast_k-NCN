#include "SampleSetFactory.h"

namespace Common {
	//TODO: should not use assignment operator, redundant copying
	// int nrLoadSamples = 0 default: all samples
	SampleSet SampleSetFactory::createSampleSet(std::string inputFilename, const int nrLoadSamples, const int nrLoadDims) {
		std::ifstream inputFile;
		inputFile.open(inputFilename); // maybe check once again

		int setNrClasses;
		int setNrDims;
		int setNrSamples;
		
		inputFile >> setNrClasses;
		inputFile >> setNrDims;
		inputFile >> setNrSamples;

		int nrOriginalDims = setNrDims;

		if (nrLoadSamples != 0 && nrLoadSamples < setNrSamples) {
			setNrSamples = nrLoadSamples;
		}
		if (nrLoadDims != 0 && nrLoadDims < setNrDims) {
			setNrDims = nrLoadDims;
		}

		SampleSet set(setNrClasses, setNrDims, setNrSamples);

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
		
		SampleDim sampleDim;
		for (int dimIndex = 0; dimIndex < sample.nrDims; dimIndex++) {
			inputFile >> sampleDim;
			sample.dims[dimIndex] = sampleDim;
		}

		float dummySampleDim; // hack to ignore the rest of dims from stream
		for (int dimIndex = sample.nrDims; dimIndex < nrOriginalDims; dimIndex++) {
			inputFile >> dummySampleDim;
		}

		return sample;
	}

}
