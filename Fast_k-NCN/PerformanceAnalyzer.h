#pragma once

#include <Windows.h>

#include "SampleSet.h"

using namespace Common;

namespace Utility {

	class PerformanceAnalyzer { //TODO: singleton
	private:
		__int64 frequency, startTime, stopTime;
	public:
		unsigned long totalTime;
		int nrClassificationErrors;
		float errorRate;

		PerformanceAnalyzer();
		~PerformanceAnalyzer();

		int* results;
		
		void startTimer();
		void stopTimer();
		void calculateError(const SampleSet& orig);
	};

}