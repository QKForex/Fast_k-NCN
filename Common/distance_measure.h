#pragma once
#ifndef COMMON_DISTANCE_MEASURE_H_
#define COMMON_DISTANCE_MEASURE_H_

#include "Sample.h"
#include "sse_util.h"

namespace Common {

float countManhattanDistance(const Sample& train, const Sample& test, const int nrDims);

float countEuclideanDistance(const Sample& train, const Sample& test, const int nrDims);

}

#endif /* COMMON_DISTANCE_MEASURE_H_ */