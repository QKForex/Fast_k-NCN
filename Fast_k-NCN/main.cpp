#include <iostream>

#include "InputReader.h"
#include "SampleSetFactory.h"
#include "Sequential_kNN.h"
#include "PerformanceAnalyzer.h"

using namespace std;
using namespace Common;
using namespace Utility;

int main(int argc, char** argv)
{
	InputReader ir;				
	
	if (!ir.readProperties(argv[1])) {
		std::cout << "Reading properties unsuccesful." << std::endl;
		exit(-1);
	};
	std::cout << "Reading properties succesful." << std::endl;

	SampleSetFactory ssf;

	SampleSet trainSet = ssf.createSampleSet(ir.trainFilename, ir.nrLoadTrainSamples);
	if (&trainSet == NULL) {
		std::cout << "Reading training samples unsuccesful." << std::endl;
		exit(-1);
	};
	std::cout << "Reading " << trainSet.nrSamples << " training samples succesful." << std::endl;

	SampleSet testSet = ssf.createSampleSet(ir.testFilename, ir.nrLoadTestSamples);
	if (&testSet == NULL) {
		std::cout << "Reading test samples unsuccesful." << std::endl;
		exit(-1);
	};
	std::cout << "Reading " << testSet.nrSamples << " test samples succesful." << std::endl;

	ofstream logfile("Logs\logkncn.txt", fstream::app); //TODO Logger class

	// -----------------------------------------------------------

	//standarizeSamples(&trainSet, &testSet);

	Sequential_kNN classifier(ir.k, trainSet.nrSamples, testSet.nrSamples); // hardcoded, should be option

	PerformanceAnalyzer pa;
	pa.startTimer();

	classifier.preprocess(trainSet, testSet);
	pa.results = classifier.classify(trainSet, testSet);

	pa.stopTimer();

	time_t rawtime; // Logger
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int error = pa.calculateError(testSet);

	float errorRate = (float) error / testSet.nrSamples * 100;

	logfile.precision(LOGGER_PRECISION); //TODO Logger

	logfile << errorRate << "%\t"
		<< error << "\t"
		<< pa.getTotalTime() << "ms\t";

	logfile << trainSet.nrSamples << "\t"
		<< testSet.nrSamples << "\t"
		<< classifier.k << "-NN\t";

	logfile << asctime(timeinfo);

	cout.precision(LOGGER_PRECISION);

	cout << errorRate << "% " << error << " " << pa.getTotalTime() << " ms " << "\n";

	cout << endl;

	return 0;
}