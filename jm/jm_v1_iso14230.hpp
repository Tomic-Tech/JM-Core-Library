/* 
 * File:   jm_v1_iso14230.hpp
 * Author: Ogilvy
 *
 * Created on 2011年12月25日, 下午11:00
 */

#ifndef JM_V1_ISO14230_HPP
#define	JM_V1_ISO14230_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm_kwp2000.hpp>
#include <jm_v1_box.hpp>

namespace jm {
namespace v1 {
class v1_iso14230 : public kwp2000{
private:
    v1_box_ptr _box;
    v1_shared_ptr _shared;
    boost::shared_ptr<v1_default<v1_iso14230> > _default;
    bool _l_line;
    int32 _send_line;
    int32 _recv_line;
    
public:
    v1_iso14230(const v1_box_ptr &box, const v1_shared_ptr &shared);
    error_code fast_init(const byte_array &data);
    error_code addr_init(int32 addr_code);
    error_code set_lines(int32 com_line, bool l_line);
    size_t send_one_frame(const uint8 *data, size_t offset, size_t count);
    size_t send_frames(const uint8 *data, size_t offset, size_t count);
    size_t read_one_frame(uint8 *data, size_t offset);
    size_t read_frames(uint8 *data, size_t offset);
    void finish_execute(bool is_finish);
    error_code set_keep_link(const uint8 *data, size_t offset, size_t count);
private:
    size_t read_one_frame(uint8 *data, size_t offset, bool is_finish);
    
};

}
}

#endif	/* JM_V1_ISO14230_HPP */

