#ifndef __JM_LOG_HPP__
#define __JM_LOG_HPP__

#include <string>
#include <boost/cstdint.hpp>

namespace JM
{

    class Log
    {
    public:
        static Log& inst();
        void write(const std::string &tag, const std::string &msg);
        void write(const std::string &tag, const boost::uint8_t *data, std::size_t count);
    private:
        Log();
        ~Log();
        void write(const std::string &str);
    private:
    };
}

#endif