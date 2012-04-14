#ifndef __JM_TYPES_HPP__
#define __JM_TYPES_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <vector>
#include <string>
#include <boost/cstdint.hpp>

#ifdef JMCORE_BUILD
#define JMCORE_API _declspec(dllexport)
#else
#define JMCORE_API _declspec(dllimport)
#endif

namespace JM
{
typedef std::vector<std::string> StringVector;
}

#endif