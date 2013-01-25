#pragma once

#include <iostream>
#include <string>

#include "SampleSet.h"
#include "Utility.h"

namespace Common {

	class SampleSetFactory
	{
	public:
		SampleSet createSampleSet(std::string inputFilename, 
			const int nrLoadSamples = 0, const int nrLoadDims = 0);
	
	private:
		Sample createSample(std::ifstream& inputFile,
			const int samIndex, const int nrSamDims, const int nrOriginalDims = 0);
	};

}