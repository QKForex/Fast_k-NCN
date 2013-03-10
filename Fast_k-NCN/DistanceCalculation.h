#pragma once

#include "Distance.h"
#include "SampleSet.h"
#include "Utility.h"

using namespace Utility;

namespace Common {

	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int nrDims);
	DistanceValue countEuclideanDistance(const Sample& train, const Sample& test, const int nrDims);
	void countDistances(const SampleSet& trainSet, const Sample& testSample, Distance* distances);

}
