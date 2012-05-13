#ifndef __JM_VEHICLE_DATABASE_H__
#define __JM_VEHICLE_DATABASE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/livedatavector.h>

namespace JM
{
    namespace Vehicle
    {
        class DatabasePrivate;
        class Database : public boost::noncopyable
        {
        public:
            static Database& inst();
            bool open(const std::string &filePath,
                const std::string &password);
            bool close();
            bool is_open();
            JMCORE_API void set_tc_catalog(const std::string &catalog);
            JMCORE_API void set_ld_catalog(const std::string &catalog);
            JMCORE_API void set_cmd_catalog(const std::string &catalog);
            JMCORE_API std::string get_text(const std::string &name);
            JMCORE_API std::string get_trouble_code(const std::string &code);
            JMCORE_API const boost::uint8_t *get_command(const std::string &name, std::size_t &count);
            JMCORE_API const boost::uint8_t *get_command(boost::int32_t id, std::size_t &count);

            template<typename MutableBufferSequence>
            std::size_t get_command(const std::string &name, const MutableBufferSequence &buff)
            {
                std::size_t count = 0;
                const boost::uint8_t *bytes = get_command(name, count);
                if (count == 0)
                    return count;
                memcpy(boost::asio::buffer_cast<boost::uint8_t*>(buff), bytes, count > boost::asio::buffer_size(buff) ? boost::asio::buffer_size(buff) : count);
                return count > boost::asio::buffer_size(buff) ? boost::asio::buffer_size(buff) : count;
            }

            template<typename MutableBufferSequence>
            std::size_t get_command(boost::int32_t id, const MutableBufferSequence &buff)
            {
                std::size_t count = 0;
                const boost::uint8_t *bytes = get_command(id, count);
                if (count == 0)
                    return count;
                memcpy(boost::asio::buffer_cast<boost::uint8_t *>(buff), bytes, count > boost::asio::buffer_size(buff) ? boost::asio::buffer_size(buff) : count);
                return count > boost::asio::buffer_size(buff) ? boost::asio::buffer_size(buff) : count;
            }

            JMCORE_API Diag::LiveDataVectorPtr get_live_data(bool showed);
        private:
            Database();
            ~Database();
        private:
            DatabasePrivate *_priv;
        };
    }
}


#endif /* __JM_VEHICLE_DATABASE_H__ */