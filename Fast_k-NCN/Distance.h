#pragma once

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
	//TODO: there may be a need for definition of constructors
}
