#ifndef __JM_DIAG_CALC_STD_OBD_TROUBLE_CODE_H__
#define __JM_DIAG_CALC_STD_OBD_TROUBLE_CODE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/types.h>

namespace JM
{
namespace Diag
{

struct CalcStdObdTroubleCode
{

    template<typename ConstBufferSequence >
    std::string operator()(const ConstBufferSequence &buff, boost::int32_t pos, boost::int32_t factor, boost::int32_t offset)
    {
        std::stringstream code;
        const boost::uint8_t *p = boost::asio::buffer_cast<const boost::uint8_t*>(buff);
        switch (p[pos * factor + offset] & 0xC0)
        {
        case 0x00:
            code << "P";
            break;
        case 0x40:
            code << "C";
            break;
        case 0x80:
            code << "B";
            break;
        default:
            code << "U";
            break;
        }
        code << std::setw(2) << std::hex << std::setfill('0') << p[pos * factor + offset];
        code << std::setw(2) << std::hex << std::setfill('0') << p[pos * factor + offset + 1];
        return code.str();
    }
};
}
}

#endif /* __JM_DIAG_CALC_STD_OBD_TROUBLE_CODE_H__ */
