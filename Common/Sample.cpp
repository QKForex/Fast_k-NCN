#include "Sample.h"
#include "util.h"

namespace Common {

	//TODO Add Comments
	//TODO IFDefs for alternative not-aligned sse Sample

	Sample::Sample() : label(0), nrDims(0)
	{
		sampleDims = NULL;
	}

	Sample::Sample(int label, int dims) : label(label), nrDims(dims)
	{
		//sampleDims = new float[nrDims + remainder_table[nrDims % 4]];
		sampleDims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
	}

	Sample::~Sample()
	{
		freeSampleDimsMemory(sampleDims, __FILE__, __LINE__);
	}

	Sample::Sample(const Sample& other) : label(other.label), nrDims(other.nrDims)
		//, sampleDims(new float[copy.nrDims + remainder_table[copy.nrDims % 4]])
	{
		sampleDims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);

		copySampleDims(other.sampleDims, other.nrDims, sampleDims);
	}

	void Sample::populateDimsFromArray(SampleDim* inArray)
	{
		freeSampleDimsMemory(sampleDims, __FILE__, __LINE__);
		sampleDims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);

		copySampleDims(inArray, nrDims, sampleDims);
	}

	void Sample::populateDimsFromFile(ifstream& infile)
	{
		//TODO Find out better way to populate dimensions - operator>> ?
		
		SampleDim x;

		freeSampleDimsMemory(sampleDims, __FILE__, __LINE__);
		sampleDims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);

		for (int i = 0; i < nrDims; i++)
		{
			infile >> x;
			sampleDims[i] = x;
		}
	}

	SampleDim* Sample::getSampleDims() const
	{
		//TODO should getter return shallow copy or deep copy?
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
			SampleDim* newSampleDims = allocateSampleDimsMemory(s.nrDims, __FILE__, __LINE__);

			copySampleDims(s.sampleDims, s.nrDims, newSampleDims);

			freeSampleDimsMemory(sampleDims, __FILE__, __LINE__);

			label = s.label;
			nrDims = s.nrDims;
			sampleDims = newSampleDims;
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
		if ((nrDims != s.nrDims) && (label != s.label))
			return false;

		if (sampleDims[0] == s.sampleDims[0])
		{
			int dimIndex = 1;

			while (dimIndex < nrDims)
			{
				if (sampleDims[dimIndex] != s.sampleDims[dimIndex])
					return false;
				dimIndex++;
			}

			return true;
		}

		return false;
	}

	bool Sample::operator!=(const Sample& s) const
	{
		return !operator==(s);
	}

	SampleDim& Sample::operator[](int i)
	{
		return sampleDims[i];
	}

	const SampleDim& Sample::operator[](int i) const
	{
		return sampleDims[i];
	}

	ostream& operator<<(ostream& out, const Sample& s)
	{
		return out << s.getSampleDims() << " " << s.getLabel() << " " << s.getNrDims();
	}
}