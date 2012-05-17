#pragma once
#ifndef COMMON_SAMPLE_H_
#define COMMON_SAMPLE_H_

#include <fstream>
#include <iostream>

using namespace std;

namespace Common {

	static int remainder_table[] = { 0, 3, 2, 1 };

	class Sample
	{
		float* sampleDims;

		int label;
		int nrDims;

	public:
		Sample();
		Sample(int label, int dims);
		~Sample();
		Sample(const Sample& copy);

		void populateDims(ifstream& infile);
		int getLabel() const;
		int getNrDims() const;
		void setLabel(int l);
		void setNrDims(int n);
		void getSampleDims(float* result) const;
		float* getSampleDims() const;

		//Sample& operator=(Sample& s);
		Sample& operator=(const Sample& s);
		//Sample& operator=(Sample* s);
		bool Sample::operator==(const Sample& s) const;
		float& operator[] (int i);
		const float& operator[](int i) const;
	};

	ostream& operator<<(ostream& out, const Sample& s);
}

#endif /* COMMON_SAMPLE_H_ */
