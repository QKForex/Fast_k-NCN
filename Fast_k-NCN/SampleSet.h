#pragma once

#include "Sample.h"

namespace Common {

	class SampleSet	{
	public:
		Sample* samples;
		int nrClasses;
		int nrDims;
		int nrSamples;

		SampleSet();
		SampleSet(int classes, int dims, int samples);
		SampleSet(const SampleSet& copy);
		~SampleSet();

		//int getNrClasses() const;
		//int getNrDims() const;
		//int getNrSamples() const;

		SampleSet& operator=(const SampleSet& rhs);
		bool operator==(const SampleSet& s) const;
		bool operator!=(const SampleSet& s) const;
		Sample& operator[] (int i);
		const Sample& operator[](int i) const;

		void swapSamples(const int samIndexToMoveToBack, const int samIndexToMoveFromBack);

		// for range-based for loop
		//inline Sample begin() const { return samples[0]; }
		//inline Sample end() const { return samples[nrDims-1]; }
		//const Sample& operator++();

		friend std::ostream& operator<<(std::ostream& out, const SampleSet& s);
		//friend Sample operator*() const;
	};
}


