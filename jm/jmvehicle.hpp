#ifndef __JM_VEHICLE_HPP__
#define __JM_VEHICLE_HPP__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <boost/thread.hpp>

namespace JM
{
	class Vehicle
	{
	public:
		static Vehicle& inst();
		void setPath(const std::string &softwarePath);
		std::string path() const;
		bool loadScript(const std::string &name, const std::string &path, const std::string &dbName);
	private:
		Vehicle();
		~Vehicle();
		bool openDB(const std::string &path, const std::string &name);
		bool openCommbox();
		bool loadLuaScript(const std::string &name, const std::string &path);
	private:
		std::string _path;
		boost::thread _commboxCloseThread;
		bool _openCommboxSuccess;
	};
}

#endif