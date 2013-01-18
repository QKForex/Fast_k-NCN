#pragma once

#include <iostream>

#include "Utility.h"

using namespace std;

namespace Utility {

	class InputReader { //TODO singleton
	public:
		char* propertiesFilename;
		char* trainFilename;
		char* testFilename;
		char* logFilename;
		ClassifierType classifier;
		int k;
		int loggerPrecision;

		int nrLoadTrainSamples;
		int nrLoadTestSamples;

		ifstream trainFile();
		ifstream testFile();
	
		InputReader();
		InputReader(char* propertiesFilename);
		~InputReader();

		bool validateInput(char* propertiesFilename);
		bool readInput(char* propertiesFilename);


	};

}