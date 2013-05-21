#pragma once

#include <cstdlib>

#include "Config.h"

using namespace Utility;

namespace Common {

	struct Distance {
		int sampleIndex;
		int sampleLabel;
		DistanceValue distValue;

		Distance(int i=-1, int l=-1, DistanceValue v=FLT_MAX)
			: sampleIndex(i), sampleLabel(l), distValue(v) {}
	};

	struct NCNDistance : Distance {
		DistanceValue ncnDistValue;

		NCNDistance(int i=-1, int l=-1, DistanceValue v=FLT_MAX, DistanceValue ncnv=FLT_MAX)
			: Distance(i, l, v), ncnDistValue(ncnv) {}
	};

	int compareDistances(const void* a, const void* b);
	int compareNCNDistances(const void* a, const void* b);

	void swapDistances(Distance* dists,
		const int samIndexToMoveToBack, const int samIndexToMoveFromBack);

	Distance randomizedSelect(Distance* A, int p, int r, int i) ;
	int randomizedPartition(Distance* A, int p, int r) ;
	int partition(Distance* A, int p, int r) ;
}
