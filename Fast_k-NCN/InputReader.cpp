#include "InputReader.h"

namespace Utility {

	InputReader::InputReader() {}

	InputReader::~InputReader() {}

	bool InputReader::readProperties(char* propertiesFilename) {
		// Properties File
		std::ifstream propertiesFile(propertiesFilename);
		if (!propertiesFile.is_open()) {
			std::cout << "File with properties does not exist." << std::endl;
			return false;
		}
		this->propertiesFilename = propertiesFilename;
		// Classifier Type
		std::string classifierName;
		propertiesFile >> classifierName;
		EnumParser<ClassifierType> parser;
		try {
			classifier = parser.ParseEnum(classifierName);
		} catch (std::exception& e) {
			std::cout << e.what() << classifierName << std::endl;
			return false;
		}
		// Number of k Nearest Neighbours
		propertiesFile >> k; 
		if (k < 1) {
			std::cout << "Number of k Nearest Neighbours must be larger than or equal to 1" << std::endl;
			return false;
		}
		// Check file with training samples
		propertiesFile >> trainFilename;
		std::cout << "train " << trainFilename << std::endl;
		std::ifstream trainFile(trainFilename);
		if (!trainFile.is_open()) {
			std::cout << "File with training samples data does not exist." << std::endl;
			return false;
		}
		trainFile.close();
		// Number of training samples
		propertiesFile >> nrLoadTrainSamples;
		if (nrLoadTrainSamples < 0) {
			std::cout << "Number of training samples to load must be larger than 0. (Default: 0 == all)" << std::endl;
			return false;
		}
		// Check file with test samples
		propertiesFile >> testFilename;
		std::cout << "test " << testFilename << std::endl;
		std::ifstream testFile(testFilename);
		if (!testFile.is_open()) {
			std::cout << "File with test samples data does not exist" << std::endl;
			return false;
		}
		testFile.close();
		// Number of test samples
		propertiesFile >> nrLoadTestSamples;
		if (nrLoadTestSamples < 0) {
			std::cout << "Number of test samples to load must be larger than 0. (Default: 0 == all)" << std::endl;
			return false;
		}
		propertiesFile.close();
		return true;
	}
}