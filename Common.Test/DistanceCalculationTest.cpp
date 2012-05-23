#include "stdafx.h"
#include "Fast_k-NCN_Common.h"
#include "config.h"

using namespace Common;

SampleDim trainSampleDims[] = {-0.3490,     0.6482,     0.2710,    -0.3151,
	1.1199,     1.5135,     0.1179,     1.2082,
	1.1918,     1.1449,     0.9726,    -0.2358,
	1.2261,    -0.2806,     0.4784,    -0.3126,
	0.1192,    -0.3256,     0.0434,    -0.3327,
	0.0022,    -0.3364,    -0.0148,    -0.3418,
	-0.0261,    -0.3483,   -0.0400,    -0.3543,
	-0.0515,    -0.3602};

SampleDim testSampleDims[] = {-0.3490,     0.6482,     0.2710,    -0.3151,
	1.1199,     1.5135,     0.1179,     1.2082,
	1.1918,     1.1449,     0.9726,    -0.2358,
	1.2261,    -0.2806,     0.4784,    -0.3126,
	0.1192,    -0.3256,     0.0434,    -0.3327,
	0.0022,    -0.3364,    -0.0148,    -0.3418,
	-0.0261,    -0.3483,   -0.0400,    -0.3543,
	-0.0515,    -0.3600};

Sample trainSample(1, 30, trainSampleDims);
Sample _testSample(1, 30, testSampleDims);



BOOST_AUTO_TEST_SUITE(DistanceCalculationTest)

BOOST_AUTO_TEST_CASE(CountManhattanDistance)
{
	Distance dist;
	dist.distValue = countManhattanDistance(trainSample, _testSample, trainSample.getNrDims());

	BOOST_CHECK(0.0002 - dist.distValue < 0.00001);

}

BOOST_AUTO_TEST_CASE(CountEuclideanDistance)
{
	Distance dist;
	dist.distValue = countEuclideanDistance(trainSample, _testSample, trainSample.getNrDims());

	BOOST_CHECK(0.0000000400 - dist.distValue < 0.000000001);

}

BOOST_AUTO_TEST_CASE(CalculateDistance)
{
	SampleSet trainSampleSet;
	ifstream trainfile("ftrain01.txt");

	trainSampleSet.populateSamples(trainfile);

	const Distance* resultDists = countDistances(trainSampleSet, _testSample);

	BOOST_CHECK(resultDists != NULL);

	delete[] resultDists;
}

BOOST_AUTO_TEST_SUITE_END()