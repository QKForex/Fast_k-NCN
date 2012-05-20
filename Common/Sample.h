#pragma once
#ifndef COMMON_SAMPLE_H_
#define COMMON_SAMPLE_H_

#include <fstream>
#include <iostream>

#include "config.h"

using namespace std;

namespace Common {

	class Sample
	{
		SampleDim* sampleDims;

		int label;
		int nrDims;

	public:
		Sample();
		Sample(int label, int dims);
		~Sample();
		Sample(const Sample& copy);

		void populateDimsFromFile(ifstream& infile);
		void populateDimsFromArray(SampleDim* inArray);
		int getLabel() const;
		int getNrDims() const;
		void setLabel(int l);
		void setNrDims(int n);
		void getSampleDims(SampleDim* result) const;
		SampleDim* getSampleDims() const;

		//Sample& operator=(Sample& s);
		Sample& operator=(const Sample& s);
		//Sample& operator=(Sample* s);
		bool Sample::operator==(const Sample& s) const;
		bool Sample::operator!=(const Sample& s) const;
		SampleDim& operator[] (int i);
		const SampleDim& operator[](int i) const;
	};

	ostream& operator<<(ostream& out, const Sample& s);
}

#endif /* COMMON_SAMPLE_H_ */
