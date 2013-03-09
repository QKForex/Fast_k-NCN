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
			char *argv[] = {"",	"../../Properties/kncn.properties"};
			
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

			classifier->learnOptimalK(trainSet, ir.largestK);
		}
	};
}