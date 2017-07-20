/*
 * ErrorTODO.cpp
 *
 *  Created on: 02.04.2012
 *      Author: sega
 */


#include <sstream>
#include "ErrorTODO.h"

using namespace std;

namespace sega {


int	The_log_level; 						// переменная хранит уровень отладочной информации





	ErTODO::ErTODO(unsigned int er, unsigned int l, const char* f) _GLIBCXX_USE_NOEXCEPT {
		err 	= er;		// номер ошибки
		line	= l;		// храним номер строки в файле
		file	= f;		// храним указатель на константу строку тмени файла
	}



	ErTODO::~ErTODO(void) _GLIBCXX_USE_NOEXCEPT {

	}

	unsigned int ErTODO::get_err(){
		return err;
	}

	unsigned int ErTODO::get_line(){
		return line;
	}

	const char* ErTODO::get_file(){
		return file;
	}

	 const char* ErTODO::what() _GLIBCXX_USE_NOEXCEPT {
		 stringstream s;
		 s << "line = " << line << " file = " << file << " err = " << err;
		return (s.str()).c_str();
	}





} // namespace sega

