/*
 * ErrorTODO.h
 *
 *  Created on: 02.04.2012
 *      Author: sega
 */

#ifndef ERRORTODO_H_
#define ERRORTODO_H_

//#include <errno.h>
#include <cerrno>
#include <syslog.h>
#include <cstring>
//#include <thread>
#include <exception>
#include <systemd/sd-journal.h>						// ситемные сообщения через systemd-jornal
//#include <easylogging++.h>
//#include <string.h>



/*
 *
 * 	переменный используемые компилятором (значение устанавливает компилятор)
 * 		__FILE__		--	имя файла ( строка содержит  путь и файл)
 * 		__LINE__		--  номер линии
 *		__DATE__		--  строка содержит дату ( дату сборки )
 *		__TIME__		--  строка содержит время сборки
 *
 */


using namespace std;
namespace sega {


// для целей отладки можем удалить ппри компиляции


#ifndef LOGLEVE

	#ifdef LOG_DBUG			// определение переменной при компилировании вставляет отладочный код



	// UCUR	- уровень логических сообщений ниже которого отправляются сообщения
	// чем важнее сообщени тем меньше его величина INFO > ERR ...

	#define LOGLEVE(UCUR,USET,...)  if((UCUR>=USET))sd_journal_print(USET,__VA_ARGS__)

	#else

	#define LOGLEVE(UCUR,USET,...)		// отладка удалена из кода

	#endif

#endif

// для сообщений которые должны постоянно присутствовать в коде дляя логирования сообщений
#ifndef LOGERR
//#define LOGERR(UCUR,USET,AR,...)  if ((UCUR >=USET)) syslog(USET | LOG_LOCAL1,AR, __VA_ARGS__)
#define LOGERR(UCUR,USET,AR,...)  if((UCUR>=USET))syslog(LOG_LOCAL1,AR,__VA_ARGS__)
#endif

/*
 * 	для инициализации класса исключения использовать данный макрос
 */
#define ER_TODO(NR)	ErTODO(NR,__LINE__, __FILE__)



extern int	The_log_level; 				// переменная хранит уровень отладочной информации



class ErTODO :public exception  {	// клас определение ошибки содержит в себе
private:
	unsigned int	err;		// номер ошибки
	unsigned int 	line;		// храним номер строки в файле
	const char *	file;		// храним указатель на константу строку имени файла

public:



	ErTODO(unsigned int, unsigned int, const char*) _GLIBCXX_USE_NOEXCEPT;				// передаем номер строки и номер файла



	virtual		~ErTODO() _GLIBCXX_USE_NOEXCEPT;
	unsigned int			get_err();						// получить номер ошибки
	unsigned int			get_line(void);					// получить номер строки с ошибкой
	const char*				get_file();						// получить имя файла

	virtual const char* what() _GLIBCXX_USE_NOEXCEPT;	// для  печати ошибки если ее не печатают

};





} // end namespace sega



#endif /* ERRORTODO_H_ */
