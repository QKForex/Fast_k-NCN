#pragma once

#include "SampleSet.h"

using namespace Common;

namespace Utility {

	class PerformanceAnalyzer {
		// start, stop time

		void startTimer();
		void stopTimer();
		int calculateError(int* const& result, const SampleSet& orig);
	};

}