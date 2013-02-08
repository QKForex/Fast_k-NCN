#pragma once

#include <iostream>
#include <string>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "SampleSet.h"
#include "Utility.h"

namespace Utility {

	class InputReader { //TODO: singleton
	public:
		po::variables_map vars;
		
		std::string propertiesFilename;
		std::string trainFilename;
		std::string testFilename;
		std::string resultFilename;
		std::string classifierName;
		ClassifierType classifier;
		int k;
		int nrLoadTrainSamples;
		int nrLoadTestSamples;
		int nrLoadSampleDims;

		std::string toCrossValidateFilename;
		std::string fromStandarizeFilename;
		std::string toStandarizeFilename;

		InputReader();
		~InputReader();

		bool readInput(int argc, char** argv);
		bool readProperties(char* propertiesFilename);
	};

}