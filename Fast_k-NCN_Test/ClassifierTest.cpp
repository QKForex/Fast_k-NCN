#include "stdafx.h"
#include "CppUnitTest.h"

#define MY_SOLUTIONDIR ($(SolutionDir))

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
		TEST_METHOD(LearnOptimalKTest) {
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

			std::unique_ptr<Classifier> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));

			//classifier->learnOptimalK(trainSet, ir.largestK);
		}

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

			std::unique_ptr<Classifier> classifier = std::unique_ptr<Sequential_kNCN>(
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

			SampleSet testSet = ssf.createSampleSet(
				ir.testFilename, ir.nrLoadTestSamples, ir.nrLoadSampleDims);
			Assert::IsNotNull(&testSet);

			std::unique_ptr<Classifier> classifier = std::unique_ptr<Sequential_kNCN>(
				new Sequential_kNCN(ir.k, trainSet.nrSamples, testSet.nrSamples));

			classifier->preprocess(trainSet, testSet);
			classifier->classify(trainSet, testSet);
		}
	};
}