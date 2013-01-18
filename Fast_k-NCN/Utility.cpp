#include "Utility.h"

using namespace std;

namespace Utility {

	bool validateInput(int argc, char** argv) {
		if (argc < 5)
		{
			cout << "Insufficient number of parameters." << endl;
			exit(-1);
		}

		// General
		const char* trainFilename = argv[1];
		const char* testFilename = argv[2];
		const char* logFilename = argv[4];

		const int nrLoadTrainSamples = 0; // 0 if all, cannot be smaller than 1
		const int nrLoadTestSamples = 0;

		// k-NN
		if ((int)argv[3] < 2)
		{
			cout << "k >= 2" << endl;
			exit(-1);
		}

		const int k = atoi(argv[3]);

		// Utility
		const int prec = 4;

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

	//void crossvalidateSamples(SampleSet* train, SampleSet* test) {
	//
	//}

	//void standardizeSamples(SampleSet* train, SampleSet* test) {
	//	const int trainSize = train->getNrSamples();
	//	const int testSize = test->getNrSamples();
	//	const int nrDims = train->getNrDims();
	//	float* meanArray = NULL;
	//	float* sdevArray = NULL;
	//	
	//	try {
	//		// Calculate arithmetic mean
	//		meanArray = new float[nrDims];
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			float sum = 0;
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				sum += (*train)[sampleIndex][dimIndex];
	//			}
	//			meanArray[dimIndex] = (float) sum / nrDims;
	//		}
	//		// Calculate standard deviation
	//		sdevArray = new float[nrDims];
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			float sum = 0;
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				sum += ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex])
	//					* ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex]);
	//			}
	//			sdevArray[dimIndex] = sqrt((float) sum / nrDims);
	//		}
	//		// Trainset standardization
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				if (sdevArray[dimIndex] != 0) {
	//					(*train)[sampleIndex][dimIndex] = ((*train)[sampleIndex][dimIndex]
	//				- meanArray[dimIndex]) / sdevArray[dimIndex];
	//				} else {
	//					(*train)[sampleIndex][dimIndex] -= meanArray[dimIndex];
	//				}
	//			}
	//		}
	//		// Testset standardization
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			for (int sampleIndex = 0; sampleIndex < testSize; sampleIndex++) {
	//				if (sdevArray[dimIndex] != 0) {
	//					(*test)[sampleIndex][dimIndex] = ((*test)[sampleIndex][dimIndex]
	//				- meanArray[dimIndex]) / sdevArray[dimIndex];
	//				} else {
	//					(*test)[sampleIndex][dimIndex] -= meanArray[dimIndex];
	//				}
	//			}
	//		}

	//		delete[] meanArray;
	//		delete[] sdevArray;
	//	} catch (bad_alloc &ba)	{
	//		cout << ba.what() << endl;
	//		if (meanArray) { delete[] meanArray; }
	//		if (sdevArray) { delete[] sdevArray; }
	//	}
	//}

}