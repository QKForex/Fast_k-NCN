#pragma once

#include "Classifier.h"

class Sequential_kNN: public Classifier {
public:
	Sequential_kNN();
	~Sequential_kNN();

	Distance* preprocess(const SampleSet& trainSet, const Sample& testSam);
	int classify(const SampleSet& trainSet, const Sample& testSam, const int k, const Distance* dists);

private:
	const Distance* findkNN(Distance* dists, int distsSize, int k);

};