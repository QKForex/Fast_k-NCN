#include "Utility.h"

using namespace std;
using namespace Common;

namespace Utility {

	//
	//	Take dataset and pick samples randomly
	//	(do not care about the number of samples of given class)
	//
	//	Perform 10-fold cross-validation seperating samples into 10 seperate files
	//
	void crossvalidateSamples(SampleSet* dataSet,
		std::string crossvalidatedFilename, const int kFold) {

		int divisor = floor((float)dataSet->nrSamples / kFold);
		std::srand(std::time(0));


		bool* chosenSamples = new bool[dataSet->nrSamples];
		for (int chosenIndex = 0; chosenIndex < dataSet->nrSamples; chosenIndex++) {
			chosenSamples[chosenIndex] = false;
		}

		for (int foldIndex = 0; foldIndex < kFold - 1; foldIndex++) {
			ofstream foldFile(crossvalidatedFilename + "_" + std::to_string(foldIndex+1) + ".txt");
			foldFile << dataSet->nrClasses << " " << dataSet->nrDims << " " << divisor << endl;

			for (int samIndex = 0; samIndex < divisor; samIndex++) {
				
				int r = ((rand()*10 + rand()) % dataSet->nrSamples);
				while (chosenSamples[r]) {
					r = ((rand()*10 + rand()) % dataSet->nrSamples);
				}
				chosenSamples[r] = true;
		
				foldFile << dataSet->samples[r].label << "\t";
				for (int dimIndex = 0; dimIndex < dataSet->nrDims; dimIndex++)
					foldFile << dataSet->samples[r][dimIndex] << "\t";

				foldFile << endl;
			}
			cout << crossvalidatedFilename + "_" + std::to_string(foldIndex+1) + ".txt" + " created." << endl;
		}

		// count all the remaining samples
		int countRemainingSamples = 0;
		for (int chosenIndex = 0; chosenIndex < dataSet->nrSamples; chosenIndex++) {
			if (!chosenSamples[chosenIndex]) { countRemainingSamples++; }
		}

		// save the remaining samples in lastFoldFile
		ofstream lastFoldFile(crossvalidatedFilename + "_" + std::to_string(kFold) + ".txt");
		lastFoldFile << dataSet->nrClasses << " " << dataSet->nrDims << " " << countRemainingSamples << endl;
		
		for (int chosenIndex = 0; chosenIndex < dataSet->nrSamples; chosenIndex++) {
			if (!chosenSamples[chosenIndex]) {
				chosenSamples[chosenIndex] = true;
				for (int dimIndex = 0; dimIndex < dataSet->nrDims; dimIndex++) {
					lastFoldFile << dataSet->samples[chosenIndex][dimIndex] << "\t";
				}

				lastFoldFile << endl;
			}
		}

		cout << crossvalidatedFilename + "_" + std::to_string(kFold) + ".txt" + " created." << endl;

		delete[] chosenSamples;
	}

	//
	//	Standarize dataset using mean and standard deviation
	//
	void standardizeSamples(SampleSet* train, SampleSet* test) {
		const int trainSize = train->nrSamples;
		const int testSize = test->nrSamples;
		const int nrDims = train->nrDims;
		vector<SampleDim> meanArray;
		vector<SampleDim> sdevArray;
		
		// Calculate arithmetic mean
		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
			float sum = 0;
			for (int samIndex = 0; samIndex < trainSize; samIndex++) {
				sum += (*train)[samIndex][dimIndex];
			}
			meanArray.push_back((SampleDim) sum / trainSize);
		}
		// Calculate standard deviation
		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
			float sum = 0;
			for (int samIndex = 0; samIndex < trainSize; samIndex++) {
				sum += ((*train)[samIndex][dimIndex] - meanArray[dimIndex])
					* ((*train)[samIndex][dimIndex] - meanArray[dimIndex]);
			}
			sdevArray.push_back(sqrt((SampleDim) sum / trainSize));
		}
		// Trainset standardization
		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
			for (int samIndex = 0; samIndex < trainSize; samIndex++) {
				if (sdevArray[dimIndex] != 0.0) {
					(*train)[samIndex][dimIndex] = ((*train)[samIndex][dimIndex]
						- meanArray[dimIndex]) / sdevArray[dimIndex];
				} else {
					(*train)[samIndex][dimIndex] -= meanArray[dimIndex];
				}
			}
		}
		// Testset standardization
		for (int dimIndex = 0; dimIndex < nrDims; dimIndex++) {
			for (int samIndex = 0; samIndex < testSize; samIndex++) {
				if (sdevArray[dimIndex] != 0.0) {
					(*test)[samIndex][dimIndex] = ((*test)[samIndex][dimIndex]
						- meanArray[dimIndex]) / sdevArray[dimIndex];
				} else {
					(*test)[samIndex][dimIndex] -= meanArray[dimIndex];
				}
			}
		}
	}
}