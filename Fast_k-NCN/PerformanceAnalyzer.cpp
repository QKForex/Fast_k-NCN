#include "PerformanceAnalyzer.h"

namespace Utility {

	PerformanceAnalyzer::PerformanceAnalyzer()
		: frequency(0), startPreprocessTime(0), stopPreprocessTime(0),
		startClassifyTime(0), stopClassifyTime(0),
		totalPreprocessTime(0), totalClassifyTime(0) {
		QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
	}

	PerformanceAnalyzer::~PerformanceAnalyzer() {}

	void PerformanceAnalyzer::startPreprocessTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &startPreprocessTime);
	}

	void PerformanceAnalyzer::stopPreprocessTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &stopPreprocessTime);
		totalPreprocessTime = (unsigned long) (((stopPreprocessTime - startPreprocessTime) * 1000) / frequency);
	}

	void PerformanceAnalyzer::startClassifyTimer() {
		
		QueryPerformanceCounter((LARGE_INTEGER*) &startClassifyTime);
	}

	void PerformanceAnalyzer::stopClassifyTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &stopClassifyTime);
		totalClassifyTime = (unsigned long) (((stopClassifyTime - startClassifyTime) * 1000) / frequency);
	}

	unsigned long PerformanceAnalyzer::getTotalTime() {
		return totalPreprocessTime + totalClassifyTime;
	}
}