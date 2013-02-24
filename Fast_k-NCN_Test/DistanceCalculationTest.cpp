#include "stdafx.h"
#include "CppUnitTest.h"

#include "DistanceCalculation.h"
#include "SampleSetFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace Fast_kNCN_Test
{	
	const static SampleDim trainSampleDims[] = {-0.3490f, 0.6482f, 0.2710f, -0.3151f,
		1.1199f, 1.5135f, 0.1179f, 1.2082f,
		1.1918f, 1.1449f, 0.9726f, -0.2358f,
		1.2261f, -0.2806f, 0.4784f, -0.3126f,
		0.1192f, -0.3256f, 0.0434f, -0.3327f,
		0.0022f, -0.3364f, -0.0148f, -0.3418f,
		-0.0261f, -0.3483f, -0.0400f, -0.3543f,
		-0.0515f, -0.3602f};;
	const static SampleDim testSampleDims[] = {-0.3490f, 0.6482f, 0.2710f, -0.3151f,
		1.1199f, 1.5135f, 0.1179f, 1.2082f,
		1.1918f, 1.1449f, 0.9726f, -0.2358f,
		1.2261f, -0.2806f, 0.4784f, -0.3126f,
		0.1192f, -0.3256f, 0.0434f, -0.3327f,
		0.0022f, -0.3364f, -0.0148f, -0.3418f,
		-0.0261f, -0.3483f, -0.0400f, -0.3543f,
		-0.0515f, -0.3600f};

	Sample trainSample(-1, 1, 30, trainSampleDims);
	Sample _testSample(-1, 1, 30, testSampleDims);


	TEST_CLASS(DistanceCalculationTest)	{


		TEST_CLASS_INITIALIZE(DistanceCalculationTestSetUp) {	

		}


		TEST_CLASS_CLEANUP(DistanceCalculationTestTearDown) 
		{

		}

public:

	TEST_METHOD(CountManhattanDistanceTest) {
		Distance dist;
		dist.distValue = countManhattanDistance(trainSample, _testSample, trainSample.nrDims);

		Assert::IsTrue(0.0002 - dist.distValue < 0.00001);
	}

	TEST_METHOD(CountEuclideanDistanceTest)	{
		Distance dist;
		dist.distValue = countEuclideanDistance(trainSample, _testSample, trainSample.nrDims);

		Assert::IsTrue(0.0000000400 - dist.distValue < 0.000000001);

	}

	TEST_METHOD(CalculateDistanceTest) {
		SampleSetFactory sf;
		SampleSet trainSampleSet = sf.createSampleSet("../Fast_k-NCN/Datasets/ftrain01.txt");

		const Distance* resultDists = countDistances(trainSampleSet, _testSample);

		Assert::IsTrue(resultDists != NULL);

		delete[] resultDists;
	}
	

	};
}