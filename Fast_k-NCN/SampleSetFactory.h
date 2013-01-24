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
			int nrLoadSamples = 0);
	
	private:
		Sample createSample(std::ifstream& inputFile,
			const int samIndex, const int nrSamDims);
	};

}