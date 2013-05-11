#pragma once

#include <cstdlib>

#include "Config.h"

using namespace Utility;

namespace Common {

	struct Distance
	{
		int sampleIndex;
		int sampleLabel;
		DistanceValue distValue;

		Distance(int i=0, int l=0, DistanceValue v=0)
			: sampleIndex(i), sampleLabel(l), distValue(v) {}
	};

	int compareDistances(const void* a, const void* b);

	void swapDistances(Distance* dists,
		const int samIndexToMoveToBack, const int samIndexToMoveFromBack);

	Distance randomizedSelect(Distance* A, int p, int r, int i) ;
	int randomizedPartition(Distance* A, int p, int r) ;
	int partition(Distance* A, int p, int r) ;
}
