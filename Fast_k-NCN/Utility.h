#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <map>

#include "Config.h"

namespace Utility {
	// Constants
	const static int LOGGER_PRECISION = 4;

	// Exceptions
	class NoSuchEnumException: public std::exception {
		virtual const char* what() const throw()
		{
			return "No such enum exists: ";
		}
	};

	// Template classes
	template <typename T>
	class EnumParser
	{
		std::map <std::string, T> enumMap;
	public:
		EnumParser(){};

		T ParseEnum(const std::string &value)
		{ 
			std::map <std::string, T>::const_iterator iValue = enumMap.find(value);
			if (iValue  == enumMap.end())
				throw NoSuchEnumException();
			return iValue->second;
		}
	};

	// Enumerations
	enum ClassifierType {KNN, SEQ_KNCN, PAR_KNCN, PT_KNCN, RAND_KNCN, 
		LIMV1_KNCN, LIMV2_KNCN, CACHE_KNCN};

	EnumParser<ClassifierType>::EnumParser()
	{
		enumMap["kNN"] = KNN;
		enumMap["Sequential_kNCN"] = SEQ_KNCN;
		enumMap["Parallel_kNCN"] = PAR_KNCN;
		enumMap["PrematureTerm_kNCN"] = PT_KNCN;
		enumMap["RandomizedSelect_kNCN"] = RAND_KNCN;
		enumMap["LimitedV1_kNCN"] = LIMV1_KNCN;
		enumMap["LimitedV2_kNCN"] = LIMV2_KNCN;
		enumMap["CacheEfficient_kNCN"] = CACHE_KNCN;
	}

	//void crossValidateSamples(SampleSet* train, SampleSet* test); //TODO: implement
	
	//void standardizeSamples(SampleSet* train, SampleSet* test); //TODO: fix

}