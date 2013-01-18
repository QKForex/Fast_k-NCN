#include "InputReader.h"

namespace Utility {

	bool InputReader::validateInput(int argc, char** argv) {
		if (argc < 5)
		{
			cout << "Insufficient number of parameters." << endl;
			exit(-1);
		}

		

		// k-NN
		if ((int)argv[3] < 2)
		{
			cout << "k >= 2" << endl;
			exit(-1);
		}


		cout << "train " << trainFilename << " ";

		ifstream trainfile(trainFilename);
		if (trainfile == NULL)
		{
			cout << "Trainfile does not exist." << endl;
			exit(-1);
		}

		ifstream testfile(testFilename);
		if (testfile == NULL)
		{
			cout << "Testfile does not exist" << endl;
			exit(-1);
		}
	}

	bool readInput(char* propertiesFilename) {
		
		validate
		
		trainFilename = argv[1];
		testFilename = argv[2];
		logFilename = argv[4];

		nrLoadTrainSamples = 0; // 0 if all, cannot be smaller than 1
		nrLoadTestSamples = 0;

		
		k = atoi(argv[3]);

		// Utility
		loggerPrecision = 4;
	}

}