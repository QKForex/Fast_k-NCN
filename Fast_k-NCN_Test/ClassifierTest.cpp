#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;
using namespace Utility;

namespace Fast_kNCN_Test {
	TEST_CLASS(ClassifierTest) {
	private:
		TEST_CLASS_INITIALIZE(DistanceCalculationTestSetUp) {	

		}

		TEST_CLASS_CLEANUP(DistanceCalculationTestTearDown) {

		}

	public:


		TEST_METHOD(ClassifierConstructorTest) {
			const int argc = 2;
			char *argv[] = {"",	"../../Properties/test.properties"};

			InputReader ir;
			Assert::IsTrue(ir.readInput(argc, argv));

			SampleSetFactory ssf;
			SampleSet trainSet = ssf.createSampleSet(
				ir.trainFilename, ir.nrLoadTrainSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&trainSet);

			SampleSet testSet = ssf.createSampleSet(
				ir.testFilename, ir.nrLoadTestSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&testSet);

			std::unique_ptr<Sequential_kNCN> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));
			
			Assert::AreEqual(classifier->k, ir.k);
			Assert::AreEqual(classifier->nrTrainSamples, trainSet.nrSamples);
			Assert::AreEqual(classifier->nrTestSamples, testSet.nrSamples);
			Assert::AreEqual(classifier->nrClassificationErrors, 0);
			Assert::AreEqual(classifier->errorRate, 0.0f);

			Assert::IsNotNull(classifier->distances);
			Assert::IsNotNull(classifier->nndists);
			Assert::IsNotNull(classifier->results);

			Assert::AreEqual(classifier->distances[0][trainSet.nrSamples-1].sampleIndex, -1);
			Assert::AreEqual(classifier->distances[0][trainSet.nrSamples-1].sampleLabel, -1);
			Assert::AreEqual(classifier->distances[0][trainSet.nrSamples-1].distValue, FLT_MAX);
			Assert::AreEqual(classifier->distances[testSet.nrSamples-1][trainSet.nrSamples-1].sampleIndex, -1);
			Assert::AreEqual(classifier->distances[testSet.nrSamples-1][trainSet.nrSamples-1].sampleLabel, -1);
			Assert::AreEqual(classifier->distances[testSet.nrSamples-1][trainSet.nrSamples-1].distValue, FLT_MAX);
			Assert::AreEqual(classifier->nndists[0][ir.k-1].sampleIndex, -1);
			Assert::AreEqual(classifier->nndists[0][ir.k-1].sampleLabel, -1);
			Assert::AreEqual(classifier->nndists[0][ir.k-1].distValue, FLT_MAX);
			Assert::AreEqual(classifier->nndists[testSet.nrSamples-1][ir.k-1].sampleIndex, -1);
			Assert::AreEqual(classifier->nndists[testSet.nrSamples-1][ir.k-1].sampleLabel, -1);
			Assert::AreEqual(classifier->nndists[testSet.nrSamples-1][ir.k-1].distValue, FLT_MAX);
			Assert::AreEqual(classifier->results[0], 0);
			Assert::AreEqual(classifier->results[testSet.nrSamples-1], 0);

		}

		TEST_METHOD(LearnOptimalKTest) {
			const int argc = 2;
			char *argv[] = {"",	"../../Properties/test.properties"};

			InputReader ir;
			Assert::IsTrue(ir.readInput(argc, argv));

			SampleSetFactory ssf;
			SampleSet trainSet = ssf.createSampleSet(
				ir.trainFilename, ir.nrLoadTrainSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&trainSet);
			Assert::AreEqual(trainSet.nrSamples, 50);

			SampleSet testSet = ssf.createSampleSet(
				ir.testFilename, ir.nrLoadTestSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&testSet);
			Assert::AreEqual(testSet.nrSamples, 5);

			std::unique_ptr<Classifier> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));

			int optimalK = classifier->learnOptimalK(trainSet, ir.largestK);
			Assert::AreEqual(optimalK, 1);

		}

		TEST_METHOD(PreprocessClassifyTest) {
			const int argc = 2;
			char *argv[] = {"",	"../../Properties/test.properties"};

			InputReader ir;
			Assert::IsTrue(ir.readInput(argc, argv));

			SampleSetFactory ssf;
			SampleSet trainSet = ssf.createSampleSet(
				ir.trainFilename, ir.nrLoadTrainSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&trainSet);
			Assert::AreEqual(trainSet.nrSamples, 50);

			SampleSet testSet = ssf.createSampleSet(
				ir.testFilename, ir.nrLoadTestSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&testSet);
			Assert::AreEqual(testSet.nrSamples, 5);

			std::unique_ptr<Sequential_kNCN> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));	

			classifier->preprocess(trainSet, testSet);
			Assert::AreEqual(classifier->distances[0][0].distValue, 11.5216f, 0.0001f);
			Assert::AreEqual(classifier->distances[0][trainSet.nrSamples-1].distValue, 4.7950f, 0.0001f);
			Assert::AreEqual(classifier->distances[testSet.nrSamples-1][0].distValue, 11.4933f, 0.0001f);
			Assert::AreEqual(classifier->distances[testSet.nrSamples-1][trainSet.nrSamples-1].distValue, 5.5617f, 0.0001f);

			classifier->classify(trainSet, testSet);
			Assert::AreEqual(classifier->nndists[0][0].distValue, 3.3038f, 0.0001f);
			Assert::AreEqual(classifier->nndists[0][classifier->k-1].distValue, 2.9256f, 0.0001f);
			Assert::AreEqual(classifier->nndists[testSet.nrSamples-1][0].distValue, 3.3060f, 0.0001f);
			Assert::AreEqual(classifier->nndists[testSet.nrSamples-1][classifier->k-1].distValue, 2.8174f, 0.0001f);
		}

		TEST_METHOD(KNCNTest) {
			const int argc = 2;
			char *argv[] = {"",	"../../Properties/test.properties"};

			InputReader ir;
			Assert::IsTrue(ir.readInput(argc, argv));

			SampleSetFactory ssf;
			SampleSet trainSet = ssf.createSampleSet(
				ir.trainFilename, 0, 0);
			Assert::IsNotNull(&trainSet);
			//Assert::AreEqual(trainSet.nrSamples, 50);

			SampleSet testSet = ssf.createSampleSet(
				ir.testFilename, 0, 0);
			Assert::IsNotNull(&testSet);
			//Assert::AreEqual(testSet.nrSamples, 5);

			std::unique_ptr<Classifier> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));

			classifier->preprocess(trainSet, testSet);

			//for (int samIndex = 0; samIndex < classifier->nrTestSamples; samIndex++) {
			//	if (classifier->k == 1) {
			//		results[samIndex] = classifier->find1NN(trainSet, testSet[samIndex], classifier->distances[samIndex]).sampleLabel;
			//	} else {
			//		(Sequential_kNCN)classifier->findkNCN(const_cast<SampleSet&> (trainSet), testSet[samIndex],
			//			classifier->distances[samIndex], classifier->nndists[samIndex], classifier->k);
			//		results[samIndex] = classifier->assignLabel(classifier->nndists[samIndex], classifier->k);
			//	}
			//}

			classifier->classify(trainSet, testSet);

			classifier->calculateErrorRate(testSet);

			Assert::AreEqual(529, classifier->nrClassificationErrors);
		}
	};
}