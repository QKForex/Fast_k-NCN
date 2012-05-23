#pragma once
#ifndef COMMON_DISTANCE_H_
#define COMMON_DISTANCE_H_

#include "config.h"

namespace Common {

	struct Distance
	{
		int sampleIndex;
		int sampleLabel;
		DistanceValue distValue;
	};

}

#endif /* COMMON_DISTANCE_H_ */
