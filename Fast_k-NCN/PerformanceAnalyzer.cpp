#include "PerformanceAnalyzer.h"

namespace Utility {

	PerformanceAnalyzer::PerformanceAnalyzer()
		: frequency(0), startTime(0), stopTime(0), totalTime(0), nrClassificationErrors(0) {}

	PerformanceAnalyzer::~PerformanceAnalyzer() {
		delete[] results; //TODO Bad practice should not be deleted here, if not created in this class
	}

	void PerformanceAnalyzer::startTimer() {
		QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
		QueryPerformanceCounter((LARGE_INTEGER*) &startTime);
	}

	void PerformanceAnalyzer::stopTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &stopTime);
		totalTime = (unsigned long) (((stopTime - startTime) * 1000) / frequency);
	}

	void PerformanceAnalyzer::calculateError(const SampleSet& orig) {
		int nrError = 0;
		// comments in this function - uncomment for more verbose logging purposes
		//ofstream file("result.txt");
		for (int origIndex = 0; origIndex < orig.nrSamples; origIndex++) {
			//file << orig[origIndex].getLabel() << " " << result[origIndex];
			if (orig[origIndex].label != results[origIndex]) {
				nrClassificationErrors++;
				//file << " error";
			}
			//file << std::endl;
		}
		//file << "Number of errors: " << nrError << std::endl;
		errorRate = (float) nrClassificationErrors / orig.nrSamples * 100;
	}

}