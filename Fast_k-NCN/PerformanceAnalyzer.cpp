#include "PerformanceAnalyzer.h"

namespace Utility {

	PerformanceAnalyzer::PerformanceAnalyzer()
		: frequency(0), startTime(0), stopTime(0), totalTime(0), nrClassificationErrors(0) {}

	PerformanceAnalyzer::~PerformanceAnalyzer() {}

	void PerformanceAnalyzer::startTimer() {
		QueryPerformanceFrequency((LARGE_INTEGER*) &frequency);
		QueryPerformanceCounter((LARGE_INTEGER*) &startTime);
	}

	void PerformanceAnalyzer::stopTimer() {
		QueryPerformanceCounter((LARGE_INTEGER*) &stopTime);
		totalTime = (unsigned long) (((stopTime - startTime) * 1000) / frequency);
	}

	unsigned long PerformanceAnalyzer::getTotalTime() const { return totalTime; }

	int PerformanceAnalyzer::calculateError(int* const& result, const SampleSet& orig) {
		const int origSize = orig.getNrSamples();
		int nrError = 0;

		// comments in this function - uncomment for more verbose logging purposes
		//ofstream file("result.txt");
		for (int origIndex = 0; origIndex < origSize; origIndex++) {
			//file << orig[origIndex].getLabel() << " " << result[origIndex];
			if (orig[origIndex].getLabel() != result[origIndex])
			{
				nrClassificationErrors++;
				//file << " error";
			}
			//file << "\n";
		}
		//file << "Number of errors: " << nrError << endl;
		return nrClassificationErrors;
	}

}