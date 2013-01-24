#pragma once

//#define LOGGING

// This should be made a template
#define MANHATTAN_DIST
//#define EUCLIDEAN_DIST

// Cannot have builds for two different architectures in on build
#define SSE 
//#define AVX

namespace Utility {
	// Type definitions
	typedef float SampleDim;
	typedef float DistanceValue;
}