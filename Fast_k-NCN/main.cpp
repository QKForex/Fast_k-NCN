#include <iostream>

#include <Windows.h>

#include "Sequential_kNN.h"

using namespace std;
using namespace Common;

int main(int argc, char** argv)
{
	// InputReader
	
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

	cout << endl;

	ofstream logfile(logFilename, fstream::app);

	// -----------------------------------------------------------

	int nrTrainSamples = trainSet.getNrSamples();
	int nrTestSamples = testSet.getNrSamples();
	int nrDims = trainSet.getNrDims();

	//standarizeSamples(&trainSet, &testSet);

	// main objects
	Sequential_kNN classifier; // hardcoded kNN classifer
	Distance** distances;
	int* results;
	

	__int64 frequency, classStart, classStop;

	QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
	QueryPerformanceCounter((LARGE_INTEGER*) &classStart);

	distances =	classifier.preprocess(trainSet, testSet);
	results = classifier.classify(trainSet, testSet, k, distances);

	QueryPerformanceCounter((LARGE_INTEGER*) &classStop);

	unsigned long classTime =
		(unsigned long) (((classStop - classStart) * 1000) / frequency);

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int error = classifier.countError(results, testSet);

	delete[] results;
	for (int i = 0; i < nrTestSamples; i++) { delete distances[i]; }
	delete[] distances;

	float errorRate = (float) error / nrTestSamples * 100;

	logfile.precision(prec); //TODO Logger

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