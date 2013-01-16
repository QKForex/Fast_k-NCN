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
	
	Classifier();
	~Classifier();

	virtual Distance* preprocess(const Common::SampleSet& trainSet, const Sample& testSam) = 0;
	virtual int classify(const Common::SampleSet& trainSet, const Sample& testSam, const int k) = 0;

private:
	const Distance find1NN(Distance* dists, int distsSize);
	int assignLabel(const Distance* dists, int distsSize);
	int countError(int* const& result, const SampleSet& orig);

};


