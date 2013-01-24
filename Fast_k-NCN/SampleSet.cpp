#include "SampleSet.h"

namespace Common {

	SampleSet::SampleSet()
		: nrClasses(0), nrDims(0), nrSamples(0) {
		samples = NULL;
	}

	SampleSet::SampleSet(int c, int d, int s)
		: nrClasses(c), nrDims(d), nrSamples(s)	{
		samples = new Sample[s];

#ifdef LOGGING
		cerr << "Allocated " << s * sizeof(Sample) << " byte(s) at address " << samples
			<< " in " << __FILE__ << ":" << __LINE__ << endl;
#endif
	}

	SampleSet::SampleSet(const SampleSet& copy) : nrClasses(copy.nrClasses),
		nrDims(copy.nrDims), nrSamples(copy.nrSamples), samples(new Sample[copy.nrSamples])	{
		if (this != &copy) {
			for (int samIndex = 0; samIndex < nrSamples; samIndex++) {
				samples[samIndex] = copy.samples[samIndex];
			}
		}
	}

	SampleSet::~SampleSet()	{
		delete[] samples;		
#ifdef LOGGING
		cerr << "Freed memory at address " << samples
			<< " in "<< __FILE__ << ":" << __LINE__ << endl;
#endif
		samples = NULL;
	}

	//int SampleSet::getNrClasses() const { return nrClasses;	}

	//int SampleSet::getNrDims() const { return nrDims; }

	//int SampleSet::getNrSamples() const { return nrSamples;	}

	SampleSet& SampleSet::operator=(const SampleSet& rhs) {
		if (this != &rhs) {
			if (samples) {
				delete[] samples;
				samples = NULL;
			}
			nrClasses = rhs.nrClasses;
			nrDims = rhs.nrDims;
			nrSamples = rhs.nrSamples;
			
			Sample* newSamples = new Sample[nrSamples];
			for (int samIndex = 0; samIndex < nrSamples; samIndex++) {
				newSamples[samIndex] = rhs[samIndex];
			}
			samples = newSamples;
		}
		return *this;
	}

	bool SampleSet::operator==(const SampleSet& s) const {
		if ((nrClasses != s.nrClasses) || (nrDims != s.nrDims) 
			|| (nrSamples != s.nrSamples)) {
			return false;
		}
		if (samples[0] == s.samples[0]) {
			int sampleIndex = 1;
			while (sampleIndex < nrSamples)	{
				if (samples[sampleIndex] != s.samples[sampleIndex]) {
					return false;
				}
				sampleIndex++;
			}
			return true;
		}
		return false;
	}

	bool SampleSet::operator!=(const SampleSet& s) const {
		return !(*this == s);
	}

	Sample& SampleSet::operator[] (int i) { return samples[i]; }

	const Sample& SampleSet::operator[](int i) const { return samples[i]; }

	std::ostream& operator<<(std::ostream& out, const SampleSet& s) {
		return out << s.nrClasses << " " << s.nrDims << " " << s.nrSamples;
	}

}
