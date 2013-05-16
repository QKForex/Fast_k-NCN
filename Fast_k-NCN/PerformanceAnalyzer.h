#pragma once

#include <Windows.h>

#include "SampleSet.h"

using namespace Common;

namespace Utility {

	class PerformanceAnalyzer {
	private:
		__int64 frequency, startPreprocessTime, stopPreprocessTime, startClassifyTime, stopClassifyTime;
	public:
		unsigned long totalPreprocessTime;
		unsigned long totalClassifyTime;

		PerformanceAnalyzer();
		~PerformanceAnalyzer();
		
		void startPreprocessTimer();
		void stopPreprocessTimer();
		void startClassifyTimer();
		void stopClassifyTimer();

		unsigned long getTotalTime();
	};

}