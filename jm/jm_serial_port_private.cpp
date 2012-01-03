#include "jm_serial_port_private.hpp"
#include "jm_error.hpp"

namespace jm {

#ifdef JM_OS_WIN
error_code serial_port_private::full_name_win(const std::string &name, std::string &result) {
    result.clear();
    if (name[0] != '\\')
        result += "\\\\.\\";

    result += name;
    if (result[4] != 'C' || result[5] != 'O' || result[6] != 'M') {
        return error::generic_error;
    }
    return error::success;
}

std::vector<std::string> serial_port_private::enumerate_device_win(const GUID *guid) {
    std::vector<std::string> vec;
    HDEVINFO dev_info;
    if ((dev_info = SetupDiGetClassDevsA(guid, NULL, NULL, DIGCF_PRESENT)) != INVALID_HANDLE_VALUE) {
        SP_DEVINFO_DATA dev_info_data;
        dev_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
        for (int32 i = 0; SetupDiEnumDeviceInfo(dev_info, i, &dev_info_data); ++i) {
            HKEY dev_key = SetupDiOpenDevRegKey(dev_info, &dev_info_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            std::string name = get_reg_key_value(dev_key, "PortName");
            std::string full_name;
            int ret = full_name_win(name, full_name);
            if (ret)
                continue;
            vec.push_back(full_name);
        }
    }
    return vec;
}

std::string serial_port_private::get_reg_key_value(HKEY key, const std::string &property) {
    DWORD size = 0;
    DWORD type;
    BYTE *buff = NULL;
    RegQueryValueExA(key, property.c_str(), NULL, NULL, NULL, &size);
    buff = new BYTE[size];
    if (RegQueryValueExA(key, property.c_str(), NULL, &type, buff, &size) == ERROR_SUCCESS) {
        std::string ret((char*)buff, size);
        delete[] buff;
        return ret;
    }
    delete[] buff;
    return std::string();
}

#endif
}