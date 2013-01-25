#pragma once

#include <fstream>
#include <iostream>

#include "Config.h"
#include "MemoryAllocation.h"
#include "Utility.h"

using namespace Utility;

namespace Common {
	//TODO: Remove getters and setters in final version, no need for such encapsulation
	//TODO: Make variables public, classes should be only containers
	class Sample {
	public:
		SampleDim* dims;
		int index;
		int label;
		int nrDims;

		Sample();
		Sample(const int i, const int l, const int n);
		Sample(const int i, const int l, const int n, const SampleDim* sampleDimsArray);
		~Sample();
		Sample(const Sample& other);

		//int getIndex() const;
		//int getLabel() const;
		//int getNrDims() const;
		//void setIndex(int i);
		//void setLabel(int l);
		//void setNrDims(int n);
		//void getSampleDims(SampleDim* result) const;
		//SampleDim* getSampleDims() const;

		//Sample& operator=(Sample& s);
		Sample& operator=(const Sample& s);
		//Sample& operator=(Sample* s);
		bool operator==(const Sample& s) const;
		bool operator!=(const Sample& s) const;
		SampleDim& operator[] (int i);
		const SampleDim& operator[](int i) const;

		friend std::ostream& operator<<(std::ostream& out, const Sample& s);

	private:
		void copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst);

	};
}
