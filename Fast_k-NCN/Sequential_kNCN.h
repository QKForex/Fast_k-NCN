#pragma once

#include "Classifier.h"

class Sequential_kNCN: public Classifier {
public:
	Sequential_kNCN();
	~Sequential_kNCN();

	Distance* preprocess(const SampleSet& trainSet, const Sample& testSam);
	int classify(const SampleSet& trainSet, const Sample& testSam, const int k, const Distance* dists);

private:
	const Distance* findkNCN(Distance* dists, int distsSize, int k);

};
