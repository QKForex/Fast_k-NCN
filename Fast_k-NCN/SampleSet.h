#pragma once

#include "Sample.h"

namespace Common {

	class SampleSet	{
	private:
		Sample* samples;
		int nrClasses;
		int nrDims;
		int nrSamples;

	public:
		SampleSet();
		SampleSet(int classes, int dims, int samples);
		SampleSet(const SampleSet& copy);
		~SampleSet();

		void populateSamples(std::ifstream& infile, int samples = 0);
		int getNrClasses() const;
		int getNrDims() const;
		int getNrSamples() const;

		SampleSet& operator=(const SampleSet& rhs);
		bool operator==(const SampleSet& s) const;
		bool operator!=(const SampleSet& s) const;
		Sample& operator[] (int i);
		const Sample& operator[](int i) const;
		
	};

	std::ostream& operator<<(std::ostream& out, const SampleSet& s);

}
