#ifndef __JM_DIAG_V1_box_H__
#define __JM_DIAG_V1_box_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>
#include <jm/diag/v1/shared.h>
#include <jm/diag/boxstream.h>

namespace JM
{
namespace Diag
{
namespace V1
{

template<typename ConstantClass>
class Box : public boost::noncopyable
{
public:
    typedef ConstantClass Constant;
    typedef boost::posix_time::microseconds MicroSeconds;
    typedef boost::posix_time::milliseconds MilliSeconds;
    typedef boost::posix_time::seconds Seconds;
    typedef boost::asio::mutable_buffer MutableBuffer;
    typedef boost::asio::const_buffer ConstBuffer;
public:

    Box(const boost::shared_ptr<Shared> &s)
    : _shared(s)
    {
    }

    virtual ~Box()
    {
    }

    static inline BoxStream &stream()
    {
        return BoxStream::inst();
    }

    template<typename DurationType>
    static inline boost::int64_t to_micro_seconds(DurationType const &duration)
    {
        return duration.total_microseconds();
    }

    template<typename DurationType>
    static inline void sleep(DurationType const &duration)
    {
        boost::this_thread::sleep(duration);
    }

    void get_link_time(boost::uint8_t type, boost::int64_t time)
    {
        if (type == Constant::SETBYTETIME)
        {
            _shared->reqByteToByte = time;
        }
        else if (type == Constant::SETRECBBOUT)
        {
            _shared->reqWaitTime = time;
        }
        else if (type == Constant::SETRECFROUT)
        {
            _shared->resByteToByte = time;
        }
        else
        {
            _shared->resWaitTime = time;
        }
    }

    virtual bool open_comm() = 0;
    virtual bool close_comm() = 0;
    virtual std::size_t read_bytes(boost::uint8_t *buff, std::size_t count) = 0;
    virtual bool set_comm_ctrl(boost::uint8_t valueOpen, boost::uint8_t valueClose) = 0;
    virtual bool set_comm_line(boost::uint8_t sendLine, boost::uint8_t recvLine) = 0;
    virtual bool set_comm_link(boost::uint8_t ctrlWord1, boost::uint8_t ctrlWord2, boost::uint8_t ctrlWord3) = 0;
    virtual bool set_comm_baud(boost::uint32_t baud) = 0;
    virtual bool set_comm_time(boost::uint8_t type, boost::int64_t microseconds) = 0;
    virtual bool set_line_level(boost::uint8_t valueLow, boost::uint8_t valueHigh) = 0;
    virtual bool commbox_delay(boost::int64_t microseconds) = 0;
    virtual bool turn_over_one_by_one() = 0;
    virtual bool stop_now(bool isStopExecute) = 0;
    virtual bool new_batch(boost::uint8_t buffID) = 0;
    virtual bool del_batch(boost::uint8_t buffID) = 0;
    virtual bool check_result(boost::int64_t microseconds) = 0;
    virtual bool keep_link(bool isRunLink) = 0;
    virtual bool send_out_data(const boost::uint8_t *buff, std::size_t count) = 0;
    virtual bool run_receive(boost::uint8_t type) = 0;
    virtual bool end_batch() = 0;
    virtual bool run_batch(boost::uint8_t *buffID, std::size_t count, bool isExecuteMany) = 0;
    virtual std::size_t read_data(boost::uint8_t *buff, std::size_t count, boost::int64_t microseconds) = 0;
    virtual bool update_buff(boost::uint8_t type, boost::uint8_t *buff) = 0;
    virtual boost::uint8_t get_abs_add(boost::uint8_t buffID, boost::uint8_t add) = 0;

protected:
    boost::shared_ptr<Shared> _shared;
};

}
}
}

#endif /* __JM_DIAG_V1_box_H__ */
