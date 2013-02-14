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

		PerformanceAnalyzer();
		~PerformanceAnalyzer();
		
		void startTimer();
		void stopTimer();
	};

}