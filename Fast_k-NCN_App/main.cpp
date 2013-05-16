#include <iostream>
#include <chrono>
#include <memory>

#include <ctime>

#include <boost/format.hpp>

#include "InputReader.h"
#include "SampleSetFactory.h"
#include "PerformanceAnalyzer.h"

#include "Sequential_kNN.h"
#include "Sequential_kNCN.h"
#include "Parallel_kNCN.h"
#include "PrematureTerm_kNCN.h"
#include "LimitedV1_kNCN.h"
#include "LimitedV2_kNCN.h"
#include "CacheEfficient_kNCN.h"

using namespace Common;
using namespace Utility;

using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("Fast_k-NCN"));

int main(int argc, char** argv) {

	DOMConfigurator::configure("../Fast_k-NCN/LoggerConfig.xml");

	//logger->closeNestedAppenders();

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
		classifier = std::unique_ptr<Sequential_kNN>(
			new Sequential_kNN(ir.k, trainSet.nrSamples, testSet.nrSamples));
		break;
	case SEQ_KNCN:
		classifier = std::unique_ptr<Sequential_kNCN>(
			new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims));
		break;
	case PAR_KNCN:
		classifier = std::unique_ptr<Parallel_kNCN>(
			new Parallel_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims));
		break;
	case PT_KNCN:
		classifier = std::unique_ptr<PrematureTerm_kNCN>(
			new PrematureTerm_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims, ir.threshold));
		break;
	case LIMV1_KNCN:
		classifier = std::unique_ptr<LimitedV1_kNCN>(
			new LimitedV1_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims, ir.percentMaxRobustRank));
		break;
	case LIMV2_KNCN:
		classifier = std::unique_ptr<LimitedV2_kNCN>(
			new LimitedV2_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims, ir.percentMaxRobustRank));
		break;
	case CACHE_KNCN:
		classifier = std::unique_ptr<CacheEfficient_kNCN>(
			new CacheEfficient_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples, trainSet.nrClasses, trainSet.nrDims));
		break;
	default:
		exit(-1);
	}

	if (ir.largestK != 0) {
		const int optimalK = classifier->learnOptimalK(trainSet, ir.largestK);
		std::cout << std::endl;
		std::cout << boost::format("Learning optimal k successful. Largest k checked: %d. Optimal k: %d.")
			% ir.largestK % optimalK << std::endl;
	}

	PerformanceAnalyzer pa;

	pa.startPreprocessTimer();
	classifier->preprocess(trainSet, testSet);
	pa.stopPreprocessTimer();
	pa.startClassifyTimer();
	classifier->classify(trainSet, testSet);
	pa.stopClassifyTimer();
	
	classifier->calculateErrorRate(testSet);

	std::chrono::time_point<std::chrono::system_clock> current_time = 
		std::chrono::system_clock::now();
	std::time_t current_time_c = std::chrono::system_clock::to_time_t(current_time);
	std::ofstream resultFile(ir.resultFilename, std::fstream::app);
	
	resultFile << boost::format("%.2f%% %7t %d %15t %6dms %t %6dms %t %6dms %36t %d %t %-20s %t %-25s %t %6d %t %-25s %t %6d %135t") 
		% classifier->errorRate % classifier->nrClassificationErrors
		% pa.totalPreprocessTime % pa.totalClassifyTime % pa.getTotalTime()
		% ir.k % ir.classifierName
		% ir.trainFilename % trainSet.nrSamples 
		% ir.testFilename % testSet.nrSamples;
	ir.classifier == PT_KNCN ? resultFile << ir.threshold << " ": resultFile << "";
	(ir.classifier == LIMV1_KNCN || ir.classifier == LIMV2_KNCN) ?  resultFile << ir.percentMaxRobustRank << "% " : resultFile << "";
	ir.classifier == LIMV1_KNCN ? resultFile << classifier->maximalRobustRank << " " : resultFile << "";
	if (ir.classifier == LIMV2_KNCN) {
		for (int i = 0; i < classifier->k; i++) { resultFile << classifier->maximalRobustRanks[i] << " "; } 
	}
	resultFile	<< "\t" << std::ctime(&current_time_c);

	std::cout << std::endl;
	std::cout << boost::format("Classifier:%30tk=%d, %s")  % ir.k % ir.classifierName << std::endl;
	std::cout << boost::format("Error rate:%30t%.2f%%") % classifier->errorRate << std::endl;
	std::cout << boost::format("Classification error:%30t%d/%d") % classifier->nrClassificationErrors % testSet.nrSamples << std::endl;
	std::cout << boost::format("Total preprocessing time:%30t%d ms") % pa.totalPreprocessTime << std::endl;
	std::cout << boost::format("Total classification time:%30t%d ms") % pa.totalClassifyTime << std::endl;
	std::cout << boost::format("Total time:%30t%d ms") % pa.getTotalTime() << std::endl;
	std::cout << std::ctime(&current_time_c);

	return 0;
}
