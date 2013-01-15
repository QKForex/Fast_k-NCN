#pragma once
#ifndef COMMON_DISTANCE_CALCULATION_H_
#define COMMON_DISTANCE_CALCULATION_H_

#include "Distance.h"
#include "SampleSet.h"
#include "Sample.h"
#include "sse_util.h"

namespace Common {

	DistanceValue countManhattanDistance(const Sample& train, const Sample& test, const int nrDims);

	DistanceValue countEuclideanDistance(const Sample& train, const Sample& test, const int nrDims);

	Distance* countDistances(const SampleSet& trainSet, const Sample& testSample);

}

#endif /* COMMON_DISTANCE_CALCULATION_H_ */