#include <iostream>
#include <chrono>
#include <memory>

#include <ctime>

#include <windows.h>

#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <log4cxx\logger.h>
#include <log4cxx\xml\domconfigurator.h>

#include "InputReader.h"
#include "SampleSetFactory.h"
#include "PerformanceAnalyzer.h"

#include "Sequential_kNN.h"
#include "Sequential_kNCN.h"
#include "Parallel_kNCN.h"
#include "RandomizedSelect_kNCN.h"
#include "LimitedV1_kNCN.h"
#include "LimitedV2_kNCN.h"
#include "CacheEfficient_kNCN.h"

namespace po = boost::program_options;

using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

using namespace Common;
using namespace Utility;

LoggerPtr loggerToFile(Logger::getLogger("MyLogger"));

int main(int argc, char** argv) {

	//DOMConfigurator::configure("logger_config.xml");

	//LOG4CXX_DEBUG(loggerToFile, "this is a debug message.");
	//LOG4CXX_INFO (loggerToFile, "this is a info message, just ignore.");
	//LOG4CXX_WARN (loggerToFile, "this is a warn message, dont worry too much.");
	//LOG4CXX_ERROR(loggerToFile, "this is a error message, something serious is happening.");
	//LOG4CXX_FATAL(loggerToFile, "this is a fatal message, crash and burn!!!");

	//loggerToFile->closeNestedAppenders();

	InputReader ir;	
	if (!ir.readInput(argc, argv)) {
		exit(-1);
	}

	SampleSetFactory ssf;
	SampleSet trainSet = ssf.createSampleSet(ir.trainFilename, ir.nrLoadTrainSamples, ir.nrLoadSampleDims);
	if (&trainSet == NULL) {
		std::cerr << "Reading training samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << boost::format("Reading %1% training samples with %2% dimensions each succesful.")
		% trainSet.nrSamples % trainSet.nrDims << std::endl;
	SampleSet testSet = ssf.createSampleSet(ir.testFilename, ir.nrLoadTestSamples, ir.nrLoadSampleDims);
	if (&testSet == NULL) {
		std::cerr << "Reading test samples unsuccesful." << std::endl;
		exit(-1);
	}
	std::cout << boost::format("Reading %1% testing samples with %2% dimensions each succesful.")
		% testSet.nrSamples % testSet.nrDims << std::endl;

	std::unique_ptr<Classifier> classifier;
	switch (ir.classifier) {
	case KNN:
		classifier = std::unique_ptr<Sequential_kNN>(new Sequential_kNN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case SEQ_KNCN:
		classifier = std::unique_ptr<Sequential_kNCN>(new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case PAR_KNCN:
		classifier = std::unique_ptr<Parallel_kNCN>(new Parallel_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case RAND_KNCN:
		classifier = std::unique_ptr<RandomizedSelect_kNCN>(new RandomizedSelect_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case LIMV1_KNCN:
		classifier = std::unique_ptr<LimitedV1_kNCN>(new LimitedV1_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case LIMV2_KNCN:
		classifier = std::unique_ptr<LimitedV2_kNCN>(new LimitedV2_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case CACHE_KNCN:
		classifier = std::unique_ptr<CacheEfficient_kNCN>(new CacheEfficient_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	default:
		exit(-1);
	}

	//classifier->learnOptimalK(trainSet, ir.largestK);

	PerformanceAnalyzer pa;
	pa.startTimer();
	classifier->preprocess(trainSet, testSet);
	classifier->classify(trainSet, testSet);
	pa.stopTimer();
	classifier->calculateErrorRate(testSet);

	std::chrono::time_point<std::chrono::system_clock> current_time = 
		std::chrono::system_clock::now();
	std::time_t current_time_c = std::chrono::system_clock::to_time_t(current_time);
	ofstream resultFile(ir.resultFilename, fstream::app);
	resultFile << boost::format("%.2f%% %7t %d %15t %6dms %25t %d %t %-20s %t %-25s %t %6d %t %-25s %t %6d %125t") 
		% classifier->errorRate % classifier->nrClassificationErrors % pa.totalTime 
		% ir.k % ir.classifierName
		% ir.trainFilename % trainSet.nrSamples 
		% ir.testFilename % testSet.nrSamples 
		<< std::ctime(&current_time_c);

	std::cout << std::endl;
	std::cout << boost::format("Classifier:%25tk=%d, %s")  % ir.k % ir.classifierName << std::endl;
	std::cout << boost::format("Error rate:%25t%.2f%%") % classifier->errorRate << std::endl;
	std::cout << boost::format("Classification error:%25t%d/%d") % classifier->nrClassificationErrors % testSet.nrSamples << std::endl;
	std::cout << boost::format("Total time:%25t%d ms") % pa.totalTime << std::endl;
	std::cout << std::ctime(&current_time_c);

	return 0;
}
