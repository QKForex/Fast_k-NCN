#pragma once

#include <iostream>
#include <string>

#include "SampleSet.h"
#include "Utility.h"

namespace Utility {

	class InputReader { //TODO: singleton
	public:
		std::string propertiesFilename;
		std::string trainFilename;
		std::string testFilename;
		std::string logFilename;
		std::string classifierName;
		ClassifierType classifier;
		int k;
		int nrLoadTrainSamples;
		int nrLoadTestSamples;
		int nrLoadSampleDims;

		InputReader();
		~InputReader();

		bool readProperties(char* propertiesFilename);
	};

}