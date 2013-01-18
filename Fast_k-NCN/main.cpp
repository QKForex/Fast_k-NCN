#include <iostream>

#include "InputReader.h"
#include "Sequential_kNCN.h"
#include "PerformanceAnalyzer.h"

using namespace std;
using namespace Common;
using namespace Utility;

int main(int argc, char** argv)
{
	InputReader ir;				
	
	if (ir.validateInput(argc, argv) {
		ir.readInput(argv[1]);
	};


	SampleSet trainSet;
	trainSet.populateSamples(trainfile, nrLoadTrainSamples);

	cout << " test " << testFilename << " ";



	SampleSet testSet;
	testSet.populateSamples(testfile, nrLoadTestSamples);

	cout << endl;

	ofstream logfile(logFilename, fstream::app);

	// -----------------------------------------------------------

	int nrTrainSamples = trainSet.getNrSamples();
	int nrTestSamples = testSet.getNrSamples();
	int nrDims = trainSet.getNrDims();

	//standarizeSamples(&trainSet, &testSet);

	Sequential_kNCN classifier(k); // hardcoded, should be option
	int* results;
	
	PerformanceAnalyzer pa;
	pa.startTimer();

	classifier.preprocess(trainSet, testSet);
	results = classifier.classify(trainSet, testSet);

	pa.stopTimer();


	time_t rawtime; // Logger
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int error = pa.calculateError(results, testSet);

	delete[] results;

	float errorRate = (float) error / nrTestSamples * 100;

	logfile.precision(prec); //TODO Logger

	logfile << errorRate << "%\t"
		<< error << "\t"
		<< pa.getTotalTime() << "ms\t";

	logfile << nrTrainSamples << "\t"
		<< nrTestSamples << "\t"
		<< k << "-NN\t";

	logfile << asctime(timeinfo);

	cout.precision(prec);

	cout << errorRate << "% " << error << " " << pa.getTotalTime() << " ms " << "\n";

	cout << endl;

	return 0;
}