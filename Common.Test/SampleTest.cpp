#include "stdafx.h"
#include "Sample.h"
#include "config.h"

#define BOOST_TEST_MODULE SampleSet testSample

using namespace Common; 

Sample testSample;
ifstream trainfile("ftrain01.txt");
SampleDim firstSampleDims[] = {-0.3490,     0.6482,     0.2710,    -0.3151,
								1.1199,     1.5135,     0.1179,     1.2082,
								1.1918,     1.1449,     0.9726,    -0.2358,
								1.2261,    -0.2806,     0.4784,    -0.3126,
								0.1192,    -0.3256,     0.0434,    -0.3327,
								0.0022,    -0.3364,    -0.0148,    -0.3418,
								-0.0261,    -0.3483,   -0.0400,    -0.3543,
								-0.0515,    -0.3602};

SampleDim secondSampleDims[] = {-0.3490,     0.6482,     0.2710,    -0.3151,
								1.1199,     1.5135,     0.1179,     1.2082,
								1.1918,     1.1449,     0.9726,    -0.2358,
								1.2261,    -0.2806,     0.4784,    -0.3126,
								0.1192,    -0.3256,     0.0434,    -0.3327,
								0.0022,    -0.3364,    -0.0148,    -0.3418,
								-0.0261,    -0.3483,   -0.0400,    -0.3543,
								-0.0515,    -0.3601}; // last dim value is different

BOOST_AUTO_TEST_SUITE(SampleTest)

BOOST_AUTO_TEST_CASE(InitializeSample)
{
	BOOST_REQUIRE(trainfile != NULL);

	//Sample testSample;

	BOOST_REQUIRE(&testSample != NULL);

	int nrClasses, nrDims, nrSamples;

	trainfile >> nrClasses;
	trainfile >> nrDims;
	trainfile >> nrSamples;

	BOOST_CHECK_EQUAL(8, nrClasses);
	BOOST_CHECK_EQUAL(30, nrDims);
	BOOST_CHECK_EQUAL(1400, nrSamples);

	int label;

	trainfile >> label;

	BOOST_CHECK_EQUAL(1, label);

	testSample.setLabel(label);

	BOOST_CHECK_EQUAL(1, testSample.getLabel());

	testSample.setNrDims(nrDims);

	BOOST_CHECK_EQUAL(nrDims, testSample.getNrDims());

	testSample.populateDimsFromFile(trainfile);

	SampleDim* testSampleDims = testSample.getSampleDims();

	BOOST_CHECK_EQUAL_COLLECTIONS(testSampleDims, testSampleDims + nrDims,
		firstSampleDims, firstSampleDims + nrDims);

}

BOOST_AUTO_TEST_CASE(ConstructorSample)
{
	BOOST_CHECK_EQUAL(1, testSample.getLabel());
	BOOST_CHECK_EQUAL(30, testSample.getNrDims());
	
	Sample testSample2(1,30);
	BOOST_CHECK_EQUAL(1, testSample2.getLabel());
	BOOST_CHECK_EQUAL(30, testSample2.getNrDims());

	testSample2.populateDimsFromArray(firstSampleDims);

	SampleDim* testSample2Dims = testSample2.getSampleDims();

	BOOST_CHECK_EQUAL_COLLECTIONS(testSample2Dims, testSample2Dims + testSample2.getNrDims(),
		firstSampleDims, firstSampleDims + (sizeof(firstSampleDims)/sizeof(SampleDim)));
}

BOOST_AUTO_TEST_CASE(CopyConstructorSample)
{
	BOOST_CHECK_EQUAL(1, testSample.getLabel());
	BOOST_CHECK_EQUAL(30, testSample.getNrDims());
	BOOST_CHECK(testSample.getSampleDims());

	SampleDim* testSampleDims = testSample.getSampleDims();

	Sample copySample(testSample);

	BOOST_CHECK_EQUAL(testSample, copySample);
	BOOST_CHECK_NE(&testSample, &copySample);

	Sample copySample2 = testSample; // invoke copy constructor

	BOOST_CHECK_EQUAL(testSample, copySample2);
	BOOST_CHECK_NE(&testSample, &copySample2);
}

BOOST_AUTO_TEST_CASE(AssignmentOperatorSample)
{
	Sample testSample3 = testSample; // invoke copy constructor

	testSample3 = testSample;

	BOOST_CHECK_EQUAL(testSample, testSample3);
	BOOST_CHECK_NE(&testSample, &testSample3);
}

BOOST_AUTO_TEST_CASE(SelfAssignmentOperatorSample)
{
	Sample testSample3 = testSample; // invoke copy constructor

	testSample3 = testSample3;

	BOOST_CHECK_EQUAL(testSample, testSample3);
	BOOST_CHECK_NE(&testSample, &testSample3);
}

BOOST_AUTO_TEST_CASE(ExplicitEqualityOperatorSample)
{
	Sample testSample3 = testSample;

	BOOST_CHECK(testSample3 == testSample);
}

BOOST_AUTO_TEST_CASE(UnequalSample)
{
	Sample otherSample(1, 30);

	otherSample.populateDimsFromArray(secondSampleDims);

	BOOST_CHECK_NE(testSample, otherSample);

	Sample otherSample2(2, 30);

	otherSample2.populateDimsFromArray(firstSampleDims);

	BOOST_CHECK_NE(testSample, otherSample2);

	SampleDim* otherSampleDims = otherSample2.getSampleDims();

	BOOST_CHECK_EQUAL_COLLECTIONS(otherSampleDims, otherSampleDims + otherSample.getNrDims(),
		firstSampleDims, firstSampleDims + (sizeof(firstSampleDims)/sizeof(SampleDim)));
}

BOOST_AUTO_TEST_SUITE_END()