#include "Sample.h"

namespace Common {

	//TODO Add Comments
	
	Sample::Sample() : index(-1), label(0), nrDims(0) {
		dims = NULL;
	}

	Sample::Sample(int index, int label, int nrDims)
		: index(index), label(label), nrDims(nrDims) {
		dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
	}

	Sample::Sample(int index, int label, int nrDims, const SampleDim* sampleDimsArray)
		: index(index), label(label), nrDims(nrDims) {
		dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
		copySampleDims(sampleDimsArray, nrDims, dims);
	}

	Sample::~Sample() {
		freeSampleDimsMemory(dims, __FILE__, __LINE__);
	}

	Sample::Sample(const Sample& other) : label(other.label), nrDims(other.nrDims) {
		dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
		copySampleDims(other.dims, other.nrDims, dims);
	}

	void Sample::populateSampleDimsFromFile(std::ifstream& infile)	{
		//TODO Find out better way to populate dimensions - operator>> ?
		SampleDim x;

		freeSampleDimsMemory(dims, __FILE__, __LINE__);
		dims = allocateSampleDimsMemory(nrDims, __FILE__, __LINE__);
		for (int i = 0; i < nrDims; i++) {
			infile >> x;
			dims[i] = x;
		}
	}

	SampleDim* Sample::getSampleDims() const {
		//TODO should getter return shallow copy or deep copy?
		
		//float* result = new float[nrDims];
		//
		//for (int i = 0; i < nrDims; i++)
		//{
		//	result[i] = dims[i];
		//}

		//return result;
		return dims;
	}

	int Sample::getIndex() const { return index; }

	int Sample::getLabel() const { return label; }

	int Sample::getNrDims() const {	return nrDims; }

	void Sample::setIndex(int i) { index = i; }

	void Sample::setLabel(int l) { label = l; }

	void Sample::setNrDims(int n) { nrDims = n; }

	Sample& Sample::operator=(const Sample& s) {
		if (this != &s)	{
			SampleDim* newSampleDims = allocateSampleDimsMemory(s.nrDims, __FILE__, __LINE__);
			copySampleDims(s.dims, s.nrDims, newSampleDims);
			freeSampleDimsMemory(dims, __FILE__, __LINE__);
			label = s.label;
			nrDims = s.nrDims;
			dims = newSampleDims;
		}

		return *this;
	}

	bool Sample::operator==(const Sample& s) const {
		if ((nrDims != s.nrDims) || (label != s.label)) {
			return false;
		}
		if (dims[0] == s.dims[0]) {
			int dimIndex = 1;
			while (dimIndex < nrDims) {
				if (dims[dimIndex] != s.dims[dimIndex])
					return false;
				dimIndex++;
			}
			return true;
		}
		return false;
	}

	bool Sample::operator!=(const Sample& s) const {
		return !operator==(s);
	}

	SampleDim& Sample::operator[](int i) { return dims[i]; }

	const SampleDim& Sample::operator[](int i) const { return dims[i]; }

	std::ostream& operator<<(std::ostream& out, const Sample& s) {
		return out << s.getSampleDims() << " " << s.getLabel() << " " << s.getNrDims();
	}

	void Sample::copySampleDims(const SampleDim* src, int nrDims, SampleDim* dst) {
		for (int i = 0; i < nrDims; i++) { dst[i] = src[i]; }
	}

}