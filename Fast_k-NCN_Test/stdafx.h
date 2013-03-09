// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include <memory>

#include "DistanceCalculation.h"
#include "InputReader.h"
#include "Sequential_kNCN.h"
#include "SampleSetFactory.h"

using namespace Common;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			template<> static std::wstring ToString<Sample> (const Sample& s) { 
				std::wstringstream ss;
				ss << s.dims << " " << s.index << " " << s.label << " " << s.nrDims;
				return ss.str();
			}
			template<> static std::wstring ToString<Sample> (const Sample* s) { 
				std::wstringstream ss;
				ss << s->dims << " " << s->index << " " << s->label << " " << s->nrDims;
				return ss.str();
			}
			template<> static std::wstring ToString<Sample> (Sample* s) { 
				std::wstringstream ss;
				ss << s->dims << " " << s->index << " " << s->label << " " << s->nrDims;
				return ss.str();
			}
			template<> static std::wstring ToString<SampleSet> (const SampleSet& s) { 
				std::wstringstream ss;
				ss << s.nrClasses << " " << s.nrDims << " " << s.nrSamples;
				return ss.str();
			}
			template<> static std::wstring ToString<SampleSet> (const SampleSet* s) { 
				std::wstringstream ss;
				ss << s->nrClasses << " " << s->nrDims << " " << s->nrSamples;
				return ss.str();
			}
			template<> static std::wstring ToString<SampleSet> (SampleSet* s) { 
				std::wstringstream ss;
				ss << s->nrClasses << " " << s->nrDims << " " << s->nrSamples;
				return ss.str();
			}
		}
	}
}