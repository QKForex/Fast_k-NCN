#pragma once

#include <Windows.h>

#include "SampleSet.h"

using namespace Common;

namespace Utility {

	class PerformanceAnalyzer { //TODO singleton
	private:
		__int64 frequency, startTime, stopTime;
		unsigned long totalTime;
		int nrClassificationErrors;

	public:
		PerformanceAnalyzer();
		~PerformanceAnalyzer();

		int* results;
		
		void startTimer();
		void stopTimer();
		unsigned long getTotalTime() const;
		int calculateError(const SampleSet& orig);
	};

}