#include "stdafx.h"
#include "SampleSet.h"
#include "config.h"

#define BOOST_TEST_MODULE SampleSet testSampleSet

using namespace Common;

SampleSet testSampleSet;

BOOST_AUTO_TEST_CASE(InitializeSampleSet)
{
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

	BOOST_REQUIRE(&testSampleSet2 != NULL);

	ifstream trainfile2("ftrain01.txt");

	testSampleSet2.populateSamples(trainfile2);

	BOOST_CHECK_EQUAL(testSampleSet, testSampleSet2);
}

BOOST_AUTO_TEST_CASE(CopyConstructorSampleSet)
{
	SampleSet copySampleSet(testSampleSet);

	BOOST_CHECK_EQUAL(testSampleSet, copySampleSet);
	BOOST_CHECK_NE(&testSampleSet, &copySampleSet);

	SampleSet copySampleSet2 = testSampleSet;

	BOOST_CHECK_EQUAL(testSampleSet, copySampleSet2);
}

BOOST_AUTO_TEST_CASE(AssignmentOperatorSampleSet)
{
	SampleSet testSampleSet5;

	BOOST_REQUIRE(&testSampleSet5 != NULL);

	testSampleSet5 = testSampleSet;

	BOOST_CHECK_EQUAL(testSampleSet, testSampleSet5);

	SampleSet testSampleSet6(8, 30, 1400);

	BOOST_REQUIRE(&testSampleSet6 != NULL);

	testSampleSet6 = testSampleSet;

	BOOST_CHECK_EQUAL(testSampleSet, testSampleSet6);
}

BOOST_AUTO_TEST_CASE(ExplicitEqualityOperatorSampleSet)
{
	SampleSet testSampleSet7 = testSampleSet;

	BOOST_REQUIRE(&testSampleSet7 != NULL);
	BOOST_CHECK(testSampleSet7 == testSampleSet);
}

BOOST_AUTO_TEST_CASE(UnequalSampleSet)
{
	SampleSet otherSampleSet;

	BOOST_REQUIRE(&otherSampleSet != NULL);

	ifstream trainfile2("ftrain02.txt");
	BOOST_REQUIRE(trainfile2 != NULL);
	BOOST_REQUIRE(&testSampleSet != NULL);

	otherSampleSet.populateSamples(trainfile2);

	BOOST_CHECK_EQUAL(8, otherSampleSet.getNrClasses());
	BOOST_CHECK_EQUAL(30, otherSampleSet.getNrDims());
	BOOST_CHECK_EQUAL(1400, otherSampleSet.getNrSamples());

	BOOST_CHECK_NE(testSampleSet, otherSampleSet);
}

BOOST_AUTO_TEST_CASE(UnequalSampleDiffNrSamplesSet)
{
	SampleSet otherSampleSet2;

	BOOST_REQUIRE(&otherSampleSet2 != NULL);

	ifstream trainfile2("ftrain02.txt");
	BOOST_REQUIRE(trainfile2 != NULL);
	BOOST_REQUIRE(&testSampleSet != NULL);

	otherSampleSet2.populateSamples(trainfile2, 100);

	BOOST_CHECK_EQUAL(8, otherSampleSet2.getNrClasses());
	BOOST_CHECK_EQUAL(30, otherSampleSet2.getNrDims());
	BOOST_CHECK_EQUAL(100, otherSampleSet2.getNrSamples());

	BOOST_CHECK_NE(testSampleSet, otherSampleSet2);
}