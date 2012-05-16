#include "Sample.h"

namespace Common {

	//TODO Add Comments
	//TODO IFDefs for alternative not-aligned sse Sample

	Sample::Sample() : label(0), nrDims(0)
	{
		sampleDims = NULL;
	}

	Sample::Sample(int label, int dims) : label(label), nrDims(dims)
	{
		//TODO Get rid of remainder_table, some modulo hack needed
		//sampleDims = new float[nrDims + remainder_table[nrDims % 4]];
		sampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
		memset(sampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));
	}

	Sample::~Sample()
	{
		//delete[] sampleDims;
		_aligned_free(sampleDims);
	}

	Sample::Sample(const Sample& copy) : label(copy.label), nrDims(copy.nrDims)
		//, sampleDims(new float[copy.nrDims + remainder_table[copy.nrDims % 4]])
	{
		sampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
		memset(sampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));

		for (int i = 0; i < nrDims; i++)
			sampleDims[i] = copy.sampleDims[i];
	}

	void Sample::populateDims(ifstream& infile)
	{
		//TODO Find out better way to populate dimensions - operator>> ?
		
		float x;

		//float* sampleDimsFirst = new float[nrDims + remainder_table[nrDims % 4]];

		sampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
		memset(sampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));

		for (int i = 0; i < nrDims; i++)
		{
			infile >> x;
			sampleDims[i] = x;
		}

		//for (int i = 0; i < nrDims; i++)
		//	sampleDims[i] = sampleDimsFirst[i];

		//delete[] sampleDimsFirst;
	}

	float* Sample::getSampleDims() const
	{
		//float* result = new float[nrDims];
		//
		//for (int i = 0; i < nrDims; i++)
		//{
		//	result[i] = sampleDims[i];
		//}

		//return result;
		return sampleDims;
	}

	int Sample::getLabel() const
	{
		return label;
	}

	int Sample::getNrDims() const
	{
		return nrDims;
	}

	void Sample::setLabel(int l) 
	{
		label = l;
	}

	void Sample::setNrDims(int n) 
	{
		nrDims = n;
	}

	Sample& Sample::operator=(const Sample& s)
	{
		if (this != &s)
		{
			//float* newSampleDims = new float[nrDims + remainder_table[nrDims % 4]];
			
			float* newSampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
			memset(newSampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));
			
			for (int i = 0; i < nrDims; i++)
				newSampleDims[i] = s[i];
			//TODO copy(s.sampleDims, s.sampleDims + s.nrDims, newSampleDims);

			if (sampleDims)
				//delete[] sampleDims;
				_aligned_free(sampleDims);

			sampleDims = newSampleDims;

			label = s.getLabel();
			nrDims = s.getNrDims();


		}

		return *this;
	}

	//Sample& Sample::operator=(Sample& s)
	//{
	//	if (this != &s)
	//	{
	//		if (sampleDims)
	//			//delete[] sampleDims;
	//			_aligned_free(sampleDims);

	//		label = s.getLabel();
	//		nrDims = s.getNrDims();

	//		//float* newSampleDims = new float[nrDims + remainder_table[nrDims % 4]];
	//		float* newSampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
	//		memset(newSampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));

	//		for (int i = 0; i < nrDims; i++)
	//			newSampleDims[i] = s[i];

	//		sampleDims = newSampleDims;
	//	}

	//	return *this;
	//}

	//Sample& Sample::operator=(Sample* s)
	//{
	//	if (this != s)
	//	{
	//		if (sampleDims)
	//			//delete[] sampleDims;
	//			_aligned_free(sampleDims);

	//		label = s->getLabel();
	//		nrDims = s->getNrDims();

	//		//float* newSampleDims = new float[nrDims + remainder_table[nrDims % 4]];
	//		float* newSampleDims = (float*) _aligned_malloc((nrDims + remainder_table[nrDims % 4]) * sizeof(float), 16);
	//		memset(newSampleDims, 0, sizeof(float) * (nrDims + remainder_table[nrDims % 4]));

	//		for (int i = 0; i < nrDims; i++)
	//			newSampleDims[i] = (*s)[i];

	//		sampleDims = newSampleDims;
	//	}

	//	return *this;
	//}

	bool Sample::operator==(const Sample& s) const
	{
		if (!(s.nrDims == nrDims) && (s.label == label))
			return false;

		if (s[0] == sampleDims[0])
		{
			int dimIndex = 1;

			while (dimIndex < nrDims)
			{
				if (sampleDims[dimIndex] != s[dimIndex])
					break;
				dimIndex++;
			}

			return true;
		}

		return false;
	}

	float& Sample::operator[](int i)
	{
		return sampleDims[i];
	}

	const float& Sample::operator[](int i) const
	{
		return sampleDims[i];
	}

	ostream& operator<<(ostream& out, const Sample& s)
	{
		return out << s.getSampleDims() << " " << s.getLabel() << " " << s.getNrDims();
	}

}