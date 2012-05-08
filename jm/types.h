#ifndef __JM_TYPES_H__
#define __JM_TYPES_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <deque>
#include <map>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/date_time.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

#ifdef JMCORE_BUILD

#ifdef BOOST_WINDOWS

#ifdef _MSC_VER
#define JMCORE_API _declspec(dllexport)
#else
#define JMCORE_API __attribute__((visibility("default")))
#endif

#else // BOOST_WINDOWS

#define JM_CORE_API __attribute__((visibility("default")))

#endif

#else // JMCORE_BUILD

#ifdef BOOST_WINDOWS

#ifdef _MSC_VER
#define JMCORE_API _declspec(dllimport)
#else
#define JMCORE_API
#endif

#else // BOOST_WINDOWS
#define JMCORE_API
#endif

#endif // JMCORE_BUILD

#ifdef _MSC_VER
#define JMCORE_HIDE
#else
#define JMCORE_HIDDEN __attribute__((visibility("hidden")))
#endif

namespace JM
{
typedef std::vector<std::string> StringVector;
typedef std::vector<boost::int32_t> IntVector;
typedef boost::shared_ptr<IntVector> IntVectorPtr;
typedef boost::array<boost::uint8_t, 0xFFF> StaticBytes;
typedef std::deque<boost::uint8_t> BytesQueue;
}


#endif
