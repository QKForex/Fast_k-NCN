#pragma once

#include <fstream>
#include <iostream>

#include "Config.h"
#include "MemoryAllocation.h"
#include "Utility.h"

using namespace std;
using namespace Utility;

namespace Common {

	class Sample
	{
		SampleDim* sampleDims;

		int label;
		int nrDims;

	public:
		Sample();
		Sample(int label, int dims);
		Sample(int label, int dims, const SampleDim* sampleDimsArray);
		~Sample();
		Sample(const Sample& copy);

		void populateDimsFromFile(ifstream& infile);
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
	};

	ostream& operator<<(ostream& out, const Sample& s);
}
