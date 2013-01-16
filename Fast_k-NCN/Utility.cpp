#include "Utility.h"

namespace Utility {

	void copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst) {
		for (int i = 0; i < nrDims; i++) { dst[i] = src[i]; }
	}

	//void crossvalidateSamples(SampleSet* train, SampleSet* test) {
	//
	//}

	//void standardizeSamples(SampleSet* train, SampleSet* test) {
	//	const int trainSize = train->getNrSamples();
	//	const int testSize = test->getNrSamples();
	//	const int nrDims = train->getNrDims();
	//	float* meanArray = NULL;
	//	float* sdevArray = NULL;
	//	
	//	try {
	//		// Calculate arithmetic mean
	//		meanArray = new float[nrDims];
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			float sum = 0;
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				sum += (*train)[sampleIndex][dimIndex];
	//			}
	//			meanArray[dimIndex] = (float) sum / nrDims;
	//		}
	//		// Calculate standard deviation
	//		sdevArray = new float[nrDims];
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			float sum = 0;
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				sum += ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex])
	//					* ((*train)[sampleIndex][dimIndex] - meanArray[dimIndex]);
	//			}
	//			sdevArray[dimIndex] = sqrt((float) sum / nrDims);
	//		}
	//		// Trainset standardization
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			for (int sampleIndex = 0; sampleIndex < trainSize; sampleIndex++) {
	//				if (sdevArray[dimIndex] != 0) {
	//					(*train)[sampleIndex][dimIndex] = ((*train)[sampleIndex][dimIndex]
	//				- meanArray[dimIndex]) / sdevArray[dimIndex];
	//				} else {
	//					(*train)[sampleIndex][dimIndex] -= meanArray[dimIndex];
	//				}
	//			}
	//		}
	//		// Testset standardization
	//		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
	//			for (int sampleIndex = 0; sampleIndex < testSize; sampleIndex++) {
	//				if (sdevArray[dimIndex] != 0) {
	//					(*test)[sampleIndex][dimIndex] = ((*test)[sampleIndex][dimIndex]
	//				- meanArray[dimIndex]) / sdevArray[dimIndex];
	//				} else {
	//					(*test)[sampleIndex][dimIndex] -= meanArray[dimIndex];
	//				}
	//			}
	//		}

	//		delete[] meanArray;
	//		delete[] sdevArray;
	//	} catch (bad_alloc &ba)	{
	//		cout << ba.what() << endl;
	//		if (meanArray) { delete[] meanArray; }
	//		if (sdevArray) { delete[] sdevArray; }
	//	}
	//}

}