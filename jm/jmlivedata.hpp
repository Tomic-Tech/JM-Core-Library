#ifndef __JM_LIVE_DATA_HPP__
#define __JM_LIVE_DATA_HPP__

#include <string>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr.hpp>

namespace JM
{
	class LiveData
	{
	public:
		LiveData();
		LiveData(const std::string &shortName,
			const std::string &content,
			const std::string &unit,
			const std::string &defaultValue,
			boost::int32_t cmdID,
			bool showed);
		LiveData(const LiveData &other);
		LiveData& operator=(const LiveData &other);
		std::string shortName() const;
		void setShortName(const std::string &shortName);
		std::string content() const;
		void setContent(const std::string &content);
		std::string unit() const;
		void setUnit(const std::string &unit);
		std::string defaultValue() const;
		void setDefaultValue(const std::string &defaultValue);
		std::string value() const;
		void setValue(const std::string &value);
		boost::int32_t cmdID() const;
		void setCmdID(boost::int32_t id);
		bool enabled() const;
		void setEnabled(bool enabled);
		bool showed() const;
		void setShowed(bool showed);
	private:
		std::string _shortName;
		std::string _content;
		std::string _unit;
		std::string _defaultValue;
		std::string _value;
		boost::int32_t _cmdID;
		bool _enabled; // If the value is FALSE, user can't choose this live data.
		bool _showed; // User can set this TRUE or FALSE to determine this live data show or not.
	};

	typedef boost::shared_ptr<LiveData> LiveDataPtr;
}

#endif