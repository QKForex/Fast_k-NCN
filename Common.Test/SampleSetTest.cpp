#include "stdafx.h"
#include "SampleSet.h"
#include "config.h"

#define BOOST_TEST_MODULE SampleSet testSampleSet

using namespace Common;

SampleSet testSampleSet;

BOOST_AUTO_TEST_CASE(InitializeSampleSet)
{
	printf("Sample size: %d Bytes\n", sizeof(Sample));
	
	ifstream trainfile("ftrain01.txt");
	BOOST_REQUIRE(trainfile != NULL);
	BOOST_REQUIRE(&testSampleSet != NULL);

	testSampleSet.populateSamples(trainfile);

	BOOST_CHECK_EQUAL(8, testSampleSet.getNrClasses());
	BOOST_CHECK_EQUAL(30, testSampleSet.getNrDims());
	BOOST_CHECK_EQUAL(1400, testSampleSet.getNrSamples());
}

BOOST_AUTO_TEST_CASE(ConstructorSampleSet)
{
	SampleSet testSampleSet2(8, 30, 1400);
	ifstream trainfile2("ftrain01.txt");

	testSampleSet2.populateSamples(trainfile2);

	BOOST_CHECK_EQUAL(testSampleSet, testSampleSet2);
}