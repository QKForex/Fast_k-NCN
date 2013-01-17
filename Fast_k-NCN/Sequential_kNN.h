#pragma once

#include "Classifier.h"

class Sequential_kNN: public Classifier {
public:
	Sequential_kNN();
	Sequential_kNN(SampleSet* train, SampleSet* test, int k);
	~Sequential_kNN();

	Distance** preprocess(const SampleSet& trainSet, const SampleSet& testSet);
	int* classify(const SampleSet& trainSet, const SampleSet& testSet, 
		const int k, Distance** dists);

private:
	const Distance* findkNN(Distance* dists, int distsSize, int k);

};