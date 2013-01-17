#pragma once

#include <fstream>
#include <iostream>

#include "Config.h"
#include "MemoryAllocation.h"
#include "Utility.h"

using namespace Utility;

namespace Common {
	//TODO Remove getters and setters in final version, no need for such encapsulation
	//TODO Make variables public, classes should be only containers
	class Sample {
	private:
		SampleDim* sampleDims;
		int label;
		int nrDims;

	public:
		Sample();
		Sample(int label, int dims);
		Sample(int label, int dims, const SampleDim* sampleDimsArray);
		~Sample();
		Sample(const Sample& other);

		void populateDimsFromFile(std::ifstream& infile);
		int getLabel() const;
		int getNrDims() const;
		void setLabel(int l);
		void setNrDims(int n);
		//void getSampleDims(SampleDim* result) const;
		SampleDim* getSampleDims() const;

		//Sample& operator=(Sample& s);
		Sample& operator=(const Sample& s);
		//Sample& operator=(Sample* s);
		bool operator==(const Sample& s) const;
		bool operator!=(const Sample& s) const;
		SampleDim& operator[] (int i);
		const SampleDim& operator[](int i) const;

	private:
		void copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst);

	};

	std::ostream& operator<<(std::ostream& out, const Sample& s);

}
