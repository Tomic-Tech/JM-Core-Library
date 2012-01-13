#ifndef JM_AUTH_HPP
#define JM_AUTH_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jm_types.hpp>
#include <string>
#include <crypt/rsa.h>

namespace jm {

class JMCORE_API auth {
public:
	static auth& instance();
	void test();
	std::string get_id_code();
	bool check_reg_dat_file(const std::string &file_path);
private:
	auth();
	CryptoPP::InvertibleRSAFunction _params;
};
}

#endif
