#pragma once

#include <iostream>
#include <fstream>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "Config.h"

namespace Utility {

	class InputReader { //TODO: singleton
	public:
		po::variables_map vars;
		
		// Filenames
		std::string propertiesFilename;
		std::string trainFilename;
		std::string testFilename;
		std::string resultFilename;
		std::string classifierName;
		
		// Classifier specific
		ClassifierType classifier;
		int k;
		int largestK;
		int nrLoadTrainSamples;
		int nrLoadTestSamples;
		int nrLoadSampleDims;
		int threshold;
		float percentMaxRobustRank;
		int nrSamplesInBlock;

		// Dataset cross-valdiation and standardization
		std::string toCrossValidateFilename;
		bool isStandardizationEnabled;

		InputReader();
		~InputReader();

		bool readInput(int argc, char** argv);
		bool readProperties(char* propertiesFilename);
	};

}