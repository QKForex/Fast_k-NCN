#include "SampleSetFactory.h"

namespace Common {
	//TODO: catch exceptions
	//TODO: should not use assignment operator, redundant copying
	// int nrLoadSamples = 0 default: all samples
	SampleSet SampleSetFactory::createSampleSet(std::string inputFilename, int nrLoadSamples) {
		std::ifstream inputFile;
		inputFile.open(inputFilename); // maybe check once again

		SampleSet set;
		inputFile >> set.nrClasses;
		inputFile >> set.nrDims;
		inputFile >> set.nrSamples;

		if (nrLoadSamples != 0 && nrLoadSamples < set.nrSamples) {
			set.nrSamples = nrLoadSamples;
		}
		if (set.samples == NULL) {
			set.samples = new Sample[set.nrSamples];
		}
		for (int samIndex = 0; samIndex < set.nrSamples; samIndex++) {
			set.samples[samIndex] = createSample(inputFile, samIndex, set.nrDims); 
		}

		inputFile.close();
		return set;
	}

	Sample SampleSetFactory::createSample(std::ifstream& inputFile, 
		const int samIndex, const int nrSamDims) {
		int samLabel;
		inputFile >> samLabel;
		Sample sample(samIndex, samLabel, nrSamDims);
		
		for (int dimIndex = 0; dimIndex < sample.nrDims; dimIndex++) {
			inputFile >> sample.dims[dimIndex];
		}
		
		return sample;
	}

}
