#include "stdafx.h"
#include "Sample.h"

using namespace Common; 

Sample testSample;
ifstream trainfile("ftrain01.txt");

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

	testSample.populateDims(trainfile);

	SampleDim firstSampleDims[] = {-0.3490,     0.6482,     0.2710,    -0.3151,
								1.1199,     1.5135,     0.1179,     1.2082,
								1.1918,     1.1449,     0.9726,    -0.2358,
								1.2261,    -0.2806,     0.4784,    -0.3126,
								0.1192,    -0.3256,     0.0434,    -0.3327,
								0.0022,    -0.3364,    -0.0148,    -0.3418,
								-0.0261,    -0.3483,   -0.0400,    -0.3543,
								-0.0515,    -0.3602};

	SampleDim* testSampleDims = testSample.getSampleDims();

	BOOST_CHECK_EQUAL_COLLECTIONS(testSampleDims, testSampleDims + nrDims,
		firstSampleDims, firstSampleDims + nrDims);

}

BOOST_AUTO_TEST_CASE(ConstructorsSample)
{
	Sample testSample2;
	
	int label;

	trainfile >> label;

	BOOST_CHECK_EQUAL(1, label);
}
