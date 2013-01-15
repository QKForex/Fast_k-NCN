#include "main.h"

using namespace Common;
using namespace std;

int main(int argc, char** argv)
{
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

	SampleSet trainSet;
	trainSet.populateSamples(trainfile, nrLoadTrainSamples);

	cout << " test " << testFilename << " ";

	ifstream testfile(testFilename);
	if (testfile == NULL)
	{
		cout << "Testfile does not exist" << endl;
		exit(-1);
	}

	SampleSet testSet;
	testSet.populateSamples(testfile, nrLoadTestSamples);

	cout << "\n";

	ofstream logfile(logFilename, fstream::app);

	// -----------------------------------------------------------

	int nrTrainSamples = trainSet.getNrSamples();
	int nrTestSamples = testSet.getNrSamples();
	int nrDims = trainSet.getNrDims();

	//standarize(&trainSet, &testSet);

	int* result = new int[nrTestSamples];

	__int64 frequency, classStart, classStop;

	QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
	QueryPerformanceCounter((LARGE_INTEGER*) &classStart);

	int sam;

	for (sam = 0; sam < nrTestSamples; sam++)
	{
		result[sam] = classify(trainSet, testSet[sam], k);
	}

	QueryPerformanceCounter((LARGE_INTEGER*) &classStop);

	unsigned long classTime =
		(unsigned long) (((classStop - classStart) * 1000) / frequency);

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int error = countError(result, testSet);

	delete[] result;

	float errorRate = (float) error / nrTestSamples * 100;

	logfile.precision(prec);

	logfile << errorRate << "%\t"
		<< error << "\t"
		<< classTime << "ms\t";

	logfile << nrTrainSamples << "\t"
		<< nrTestSamples << "\t"
		<< k << "-NN\t";

	logfile << asctime(timeinfo);

	cout.precision(prec);

	cout << errorRate << "% " << error << " " << classTime << " ms " << "\n";

	cout << endl;

	return 0;
}