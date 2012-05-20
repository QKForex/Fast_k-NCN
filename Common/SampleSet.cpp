#include "SampleSet.h"

namespace Common {

	SampleSet::SampleSet() : nrClasses(0), nrDims(0), nrSamples(0)
	{
		samples = NULL;
	}

	SampleSet::SampleSet(int c, int d, int s) : nrClasses(c), nrDims(d), nrSamples(s)
	{
		samples = new Sample[s];
	}


	SampleSet::SampleSet(const SampleSet& copy) : nrClasses(copy.nrClasses),
		nrDims(copy.nrDims), nrSamples(copy.nrSamples), samples(new Sample[copy.nrSamples])
	{
		if (this != &copy)
		{
			for (int i = 0; i < nrSamples; i++)
				samples[i] = copy.samples[i];
		}
	}

	SampleSet::~SampleSet()
	{
		delete[] samples;
		samples = NULL;
	}

	void SampleSet::populateSamples(ifstream& infile, int s)
	{
		//TODO Refactor populaton of SampleSet

		infile >> nrClasses;
		infile >> nrDims;
		infile >> nrSamples;

		if (s != 0)
			nrSamples = s;

		if (samples == NULL)
			samples = new Sample[nrSamples];

		for (int i = 0; i < nrSamples; i++)
		{
			int label = 0;

			infile >> label;

			samples[i].setLabel(label);
			samples[i].setNrDims(nrDims);

			samples[i].populateDimsFromFile(infile);

			//TODO check this implementation
			//Sample dataobj(label, nrDims);

			//dataobj.populateDims(infile);

			//samples[i] = dataobj;
		}
	}

	int SampleSet::getNrClasses() const
	{
		return nrClasses;
	}

	int SampleSet::getNrDims() const
	{
		return nrDims;
	}

	int SampleSet::getNrSamples() const
	{
		return nrSamples;
	}

	SampleSet& SampleSet::operator=(const SampleSet& rhs)
	{
		if (this != &rhs)
		{
			if (samples) {
				delete[] samples;
				samples = NULL;
			}

			nrClasses = rhs.getNrClasses();
			nrDims = rhs.getNrDims();
			nrSamples = rhs.getNrSamples();

			Sample* newSamples = new Sample[nrSamples];

			for (int i = 0; i < nrSamples; i++)
				newSamples[i] = rhs[i];

			samples = newSamples;
		}

		return *this;
	}

	bool SampleSet::operator==(const SampleSet& s) const
	{
		if ((nrClasses != s.nrClasses) && (nrDims != s.nrDims) 
			&& (nrSamples != s.nrSamples)) {
			return false;
		}

		if (samples[0] == s.samples[0])
		{
			int sampleIndex = 1;

			while (sampleIndex < nrSamples)
			{
				if (samples[sampleIndex] != s.samples[sampleIndex]) {
					return false;
				}
					
				sampleIndex++;
			}

			return true;
		}

		return false;
	}

	bool SampleSet::operator!=(const SampleSet& s) const
	{
		return !(*this == s);
	}

	Sample& SampleSet::operator[] (int i)
	{
		return samples[i];
	}

	const Sample& SampleSet::operator[](int i) const
	{
		return samples[i];
	}

	ostream& operator<<(ostream& out, const SampleSet& s)
	{
		return out << s.getNrClasses() << " " << s.getNrDims() << " " << s.getNrSamples();
	}
}
