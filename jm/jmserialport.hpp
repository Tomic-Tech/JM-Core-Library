#ifndef __JM_SERIAL_PORT_HPP_
#define __JM_SERIAL_PORT_HPP_

#ifdef _MSC_VER
#pragma once
#endif

#include "jmerror.h"
#include "jmserialporttype.h"
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/buffer.hpp>

#ifdef BOOST_WINDOWS
#include <Windows.h>
#include <ObjBase.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#include <Dbt.h>
#else
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#endif

namespace JM
{
    class SerialPort
    {
    public:
        SerialPort();
        SerialPort(const std::string &name);
        virtual ~SerialPort();
        std::string portName() const;
        boost::int32_t setPortName(const std::string &name);
        boost::int32_t baudrate();
        boost::int32_t setBaudrate(boost::int32_t baudrate);
        boost::uint8_t databits();
        boost::int32_t setDatabits(boost::uint8_t databits);
        boost::int32_t parity();
        boost::int32_t setParity(boost::int32_t parity);
        boost::int32_t stopbits();
        boost::int32_t setStopbits(boost::int32_t stopbits);
        boost::int32_t flowControl();
        boost::int32_t setFlowControl(boost::int32_t flowControl);
		
		template<typename ConstBufferSequence>
        std::size_t write(const ConstBufferSequence &data);

		template<typename MutableBufferSequence>
        std::size_t read(const MutableBufferSequence &data);

        boost::posix_time::time_duration readTimeout();

		template<typename DurationType>
        boost::int32_t setReadTimeout(DurationType const &time);

        boost::posix_time::time_duration writeTimeout();

		template<typename DurationType>
        boost::int32_t setWriteTimeout(DurationType const &time);

        bool isOpen();
        boost::int32_t open();
        boost::int32_t close();
        boost::int32_t flush();
        boost::int32_t discardInBuffer();
        boost::int32_t discardOutBuffer();
        std::size_t bytesAvailable();
        boost::int32_t setDtr(bool set);
        boost::int32_t setRts(bool set);
        static std::vector<std::string> getSystemPorts();
    private:
        void init();
        void platformInit();
        void platformDestroy();
#ifdef BOOST_WINDOWS
        static boost::int32_t fullNameWin(const std::string &name, std::string &result);
        static std::vector<std::string> enumerateDeviceWin(const GUID *guid);
        static std::string getRegKeyValue(HKEY key, const std::string &property);
#else
        boost::int32_t _setBaudrate(tcflag_t baud);
        boost::int32_t _setDatabits(tcflag_t bits);
#endif
    private:
        // Data
        std::string _portName;
        boost::posix_time::time_duration _readTimeout;
        boost::posix_time::time_duration _writeTimeout;
        boost::int32_t _parity;
        boost::int32_t _stopbits;
        boost::int32_t _flowControl;
        boost::int32_t _baudrate;
        bool _isMultiSetting;
        boost::uint8_t _databits;
		boost::mutex _mutex;
#ifdef BOOST_WINDOWS
        boost::int64_t _bytesToWrite;
        HANDLE _handle;
        COMMCONFIG _commConfig;
        COMMTIMEOUTS _commTimeouts;
        DWORD _eventMask;
#else
        int fd;
        struct termios _commConfig;
        struct termios _oldTermios;
        struct timeval _timeout;
        struct timeval _copyTimeout;
#endif
    };
}

#ifdef BOOST_WINDOWS
#include <jm/jmserialportwin.hpp>
#else
#include <jm/jmserialportposix.hpp>
#endif

#endif