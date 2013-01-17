#pragma once

#include <algorithm>
#include <climits>

#include <time.h>
#include <windows.h>

#include "Distance.h"
#include "DistanceCalculation.h"
#include "SampleSet.h"

using namespace std;
using namespace Common;

//
//	Abstract base class for Classifiers
//
class Classifier {
public:
	SampleSet* trainSet;
	SampleSet* testSet;
	int k;
	
	virtual ~Classifier() {}; // cannot implmenet pure virtual destructor
	virtual Distance** preprocess(const SampleSet& trainSet, const SampleSet& testSet) = 0;
	virtual int* classify(const SampleSet& trainSet, const SampleSet& testSet,
		const int k, Distance** dists) = 0;

	int countError(int* const& result, const SampleSet& orig);

protected:
	Classifier();
	Classifier(SampleSet* train, SampleSet* test, int k);

	const Distance find1NN(Distance* dists, int distsSize);
	int assignLabel(const Distance* dists, int distsSize);

};


