#pragma once

#include <vector>

#include <ctime>

#include "SampleSet.h"

using namespace Common;

namespace Utility {

	void crossvalidateSamples(SampleSet* dataSet,
		std::string crossvalidatedFilename, const int kFold);
	void standardizeSamples(SampleSet* train, SampleSet* test);

}