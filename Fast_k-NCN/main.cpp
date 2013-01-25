#include <iostream>
#include <chrono>
#include <ctime>

#include "InputReader.h"
#include "SampleSetFactory.h"
//#include "Logger.h"
//#include "OutputWriter.h"
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
		std::cerr << "Reading properties unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading properties for " << ir.classifierName << " from " << ir.propertiesFilename << " succesful." << std::endl;

	SampleSetFactory ssf;
	SampleSet trainSet = ssf.createSampleSet(ir.trainFilename, ir.nrLoadTrainSamples);
	if (&trainSet == NULL) {
		std::cerr << "Reading training samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading " << trainSet.nrSamples << " training samples succesful." << std::endl;
	SampleSet testSet = ssf.createSampleSet(ir.testFilename, ir.nrLoadTestSamples);
	if (&testSet == NULL) {
		std::cerr << "Reading test samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << "Reading " << testSet.nrSamples << " test samples succesful." << std::endl;

	ofstream logfile(ir.logFilename, fstream::app); //TODO: Logger class

	Classifier* classifier = NULL;
	switch (ir.classifier) {
	case KNN:
		classifier = new Sequential_kNN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case SEQ_KNCN:
		classifier = new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case PAR_KNCN:
		classifier = new Parallel_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case RAND_KNCN:
		classifier = new RandomizedSelect_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case LIMV1_KNCN:
		classifier = new LimitedV1_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case LIMV2_KNCN:
		classifier = new LimitedV2_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	case CACHE_KNCN:
		classifier = new CacheEfficient_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples);
		break;
	default:
		exit(-1);
	}

	PerformanceAnalyzer pa;
	pa.startTimer();
	classifier->preprocess(trainSet, testSet);
	pa.results = classifier->classify(trainSet, testSet);
	pa.stopTimer();
	pa.calculateError(testSet);

	logfile.precision(LOGGER_PRECISION);
	logfile << pa.errorRate << "%\t" << pa.nrClassificationErrors << "\t" << pa.totalTime << "ms\t";
	logfile << trainSet.nrSamples << "\t" << testSet.nrSamples << "\t";
	logfile	<< ir.k << "\t" << ir.classifierName << "\t";

	std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
	std::time_t current_time_c = std::chrono::system_clock::to_time_t(current_time);
	logfile << std::ctime(&current_time_c) << std::endl;

	std::cout.precision(LOGGER_PRECISION);
	std::cout << "Error rate: " << pa.errorRate << "%" << std::endl;
	std::cout << "Classification error: " << pa.nrClassificationErrors << "/" << testSet.nrSamples << std::endl;
	std::cout << "Total time: " << pa.totalTime << " ms" << std::endl;
	std::cout << std::ctime(&current_time_c) << std::endl;

	return 0;
}
