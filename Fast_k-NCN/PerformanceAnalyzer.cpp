#include "PerformanceAnalyzer.h"


//int PerformanceAnalyzer::calculateError(int* const& result, const SampleSet& orig) {
//	const int origSize = orig.getNrSamples();
//	int nrError = 0;
//
//	// comments in this function - uncomment for more verbose logging purposes
//	//ofstream file("result.txt");
//	for (int origIndex = 0; origIndex < origSize; origIndex++) {
//		//file << orig[origIndex].getLabel() << " " << result[origIndex];
//		if (orig[origIndex].getLabel() != result[origIndex])
//		{
//			error++;
//			//file << " error";
//		}
//		//file << "\n";
//	}
//	//file << "Number of errors: " << nrError << endl;
//	return error;
//}