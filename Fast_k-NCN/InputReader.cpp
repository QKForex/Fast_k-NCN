#include "InputReader.h"

namespace Utility {

	InputReader::InputReader() {}

	InputReader::~InputReader() {}

	bool InputReader::readProperties(char* propertiesFilename) {
		// GENERAL properties
		// Properties File
		std::ifstream propertiesFile(propertiesFilename);
		if (!propertiesFile.is_open()) {
			std::cerr << "File with properties does not exist." << std::endl;
			return false;
		}
		this->propertiesFilename = propertiesFilename;
		// Classifier Type
		propertiesFile >> classifierName;
		EnumParser<ClassifierType> parser;
		try {
			classifier = parser.ParseEnum(classifierName);
		} catch (std::exception& e) {
			std::cerr << e.what() << classifierName << std::endl;
			return false;
		}
		// Number of k Nearest Neighbours
		propertiesFile >> k; 
		if (k < 1) {
			std::cerr << "Number of k Nearest Neighbours must be larger than or equal to 1" << std::endl;
			return false;
		}
		// Check file with training samples
		propertiesFile >> trainFilename;
		std::cout << "Train file: " << trainFilename << std::endl;
		std::ifstream trainFile(trainFilename);
		if (!trainFile.is_open()) {
			std::cerr << "File with training samples data does not exist." << std::endl;
			return false;
		}
		trainFile.close();
		// Number of training samples
		propertiesFile >> nrLoadTrainSamples;
		if (nrLoadTrainSamples < 0) {
			std::cerr << "Number of training samples to load must be larger than 0. (Default: 0 == all)" << std::endl;
			return false;
		}
		// Check file with test samples
		propertiesFile >> testFilename;
		std::cout << "Test file: " << testFilename << std::endl;
		std::ifstream testFile(testFilename);
		if (!testFile.is_open()) {
			std::cerr << "File with test samples data does not exist" << std::endl;
			return false;
		}
		testFile.close();
		// Number of test samples to read
		propertiesFile >> nrLoadTestSamples;
		if (nrLoadTestSamples < 0) {
			std::cerr << "Number of test samples to read must be larger than 0. (Default: 0 == all)" << std::endl;
			return false;
		}
		// Number of dimensions to read
		propertiesFile >> nrLoadSampleDims;
		if (nrLoadSampleDims < 0) {
			std::cerr << "Number of test sample dims to read must be larger than 0. (Default: 0 == all)" << std::endl;
			return false;
		}
		// Log filename
		propertiesFile >> logFilename;
		// SPECIFIC properties
		//TODO: here properties for specific classifiers
		propertiesFile.close();
		return true;
	}
}