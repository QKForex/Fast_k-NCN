#include "PerformanceAnalyzer.h"

namespace Utility {

	PerformanceAnalyzer::PerformanceAnalyzer()
		: frequency(0), startTime(0), stopTime(0), totalTime(0) {}

	PerformanceAnalyzer::~PerformanceAnalyzer() {}

	void PerformanceAnalyzer::startTimer() {
		QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
		QueryPerformanceCounter((LARGE_INTEGER*) &startTime);
	}

	void PerformanceAnalyzer::stopTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &stopTime);
		totalTime = (unsigned long) (((stopTime - startTime) * 1000) / frequency);
	}
}