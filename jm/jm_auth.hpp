#ifndef JM_AUTH_HPP
#define JM_AUTH_HPP

#ifdef _MSC_VER
#pragma once
#endif

#include <jm/jm_types.hpp>
#include <string>
#include <vector>
#include <crypt/rsa.h>

namespace jm {

class JMCORE_API auth {
public:
	static auth& instance();
	void write_register(const std::string &reg);
	void set_dat_path(const std::string &path);
	std::string get_id_code();
	std::string de_id_code();
	std::string de_commbox_id();
	std::string de_first_time();
	std::string de_expire_time();
	std::string de_db();
private:
	auth();
	std::vector<std::string> decrypt();
	CryptoPP::InvertibleRSAFunction _params;
	std::string _dat_path;
};
}

#endif
