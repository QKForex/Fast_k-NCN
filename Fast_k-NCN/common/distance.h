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

		Distance(int i=0, int l=0, DistanceValue v=0)
			: sampleIndex(i), sampleLabel(l), distValue(v) {}
	};

}

#endif /* COMMON_DISTANCE_H_ */
