#pragma once
#ifndef COMMON_SAMPLESET_H_
#define COMMON_SAMPLESET_H_

#include "Sample.h"

namespace Common {

	class SampleSet
	{
		Sample* samples;

		int nrClasses;
		int nrDims;
		int nrSamples;

	public:
		SampleSet();
		SampleSet(int classes, int dims, int samples);
		SampleSet(const SampleSet& copy);
		~SampleSet();

		void populateSamples(ifstream& infile, int samples = 0);
		int getNrClasses() const;
		int getNrDims() const;
		int getNrSamples() const;

		SampleSet& operator=(const SampleSet& rhs);
		Sample& operator[] (int i);
		const Sample& operator[](int i) const;
		
	};

	ostream& operator<<(ostream& out, const SampleSet& s);

}
#endif /* COMMON_SAMPLESET_H_ */


