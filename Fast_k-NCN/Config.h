#pragma once

// This should be made a template
#define MANHATTAN_DIST //TODO: implement AVX version
//#define EUCLIDEAN_DIST //TODO: Slow should optimize with SSE/AVX

// Cannot have builds for two different architectures in one build
//#define SSE 
#define AVX

namespace Utility {
	// Type definitions
	typedef float SampleDim;
	typedef float DistanceValue;
}