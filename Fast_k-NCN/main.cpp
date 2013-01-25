#include <iostream>

#include "InputReader.h"
#include "SampleSetFactory.h"
#include "Logger.h"
#include "OutputWriter.h"
#include "PerformanceAnalyzer.h"

#include "Sequential_kNN.h"
#include "Sequential_kNCN.h"
#include "Parallel_kNCN.h"
#include "RandomizedSelect_kNCN.h"
#include "LimitedV1_kNCN.h"
#include "LimitedV2_kNCN.h"
#include "CacheEfficient_kNCN.h"

using namespace std;
using namespace Common;
using namespace Utility;

int main(int argc, char** argv)
{
	InputReader ir;				
	if (!ir.readProperties(argv[1])) {
		std::cout << "Reading properties unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading properties succesful." << std::endl;

	SampleSetFactory ssf;
	SampleSet trainSet = ssf.createSampleSet(ir.trainFilename, ir.nrLoadTrainSamples);
	if (&trainSet == NULL) {
		std::cout << "Reading training samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading " << trainSet.nrSamples << " training samples succesful." << std::endl;
	SampleSet testSet = ssf.createSampleSet(ir.testFilename, ir.nrLoadTestSamples);
	if (&testSet == NULL) {
		std::cout << "Reading test samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading " << testSet.nrSamples << " test samples succesful." << std::endl;

	ofstream logfile(ir.logFilename, fstream::app); //TODO: Logger class

	Classifier* classifier = NULL;
	switch (ir.classifier) {
	case KNN:
		classifier = new Sequential_kNN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case SEQ_KNCN:
		classifier = new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case PAR_KNCN:
		classifier = new Parallel_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case RAND_KNCN:
		classifier = new RandomizedSelect_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case LIMV1_KNCN:
		classifier = new LimitedV1_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case LIMV2_KNCN:
		classifier = new LimitedV2_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	case CACHE_KNCN:
		classifier = new CacheEfficient_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
	}

	PerformanceAnalyzer pa;
	pa.startTimer();
	classifier->preprocess(trainSet, testSet);
	pa.results = classifier->classify(trainSet, testSet);
	pa.stopTimer();
	pa.calculateError(testSet);

	logfile.precision(LOGGER_PRECISION); //TODO: Logger
	logfile << pa.errorRate << "%\t" << pa.nrClassificationErrors << "\t" << pa.totalTime << "ms\t";
	logfile << trainSet.nrSamples << "\t" << testSet.nrSamples << "\t"	<< ir.k << "-NN\t";

	time_t rawtime; // Logger, OutputWriter
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	logfile << asctime(timeinfo);

	cout.precision(LOGGER_PRECISION);
	cout << pa.errorRate << "% " << pa.nrClassificationErrors << " " << pa.totalTime << " ms " << "\n";
	cout << endl;

	return 0;
}
