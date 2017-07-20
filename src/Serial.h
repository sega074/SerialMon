/*
 * Serial.h
 *
 *  Created on: 14 сент. 2015 г.
 *      Author: sega
 */

/*
 * Клас работы с последовательным портом
 *
 *
 */

#ifndef SRC_MON_CLIENT_DRV_SERIAL_H_
#define SRC_MON_CLIENT_DRV_SERIAL_H_

#include <sys/types.h>				// используется для открытия и работы с файлами
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/epoll.h>				// используется для взаимодействия со структурой epoll
#include <sys/select.h>				// для использования select

#include <strings.h>				// для обнуления областей памяти

#include <termio.h>					// используется для работы с линиями последовательных интерфейсов
#include <errno.h>
#include <unistd.h>
#include <string>
#include <new>
#include <stdexcept>				// для обработки исключительных ситуаций времени исполнения
#include "ErrorTODO.h"



using namespace std;
namespace sega {





/*
 * Определение для класса работы с последовательным портом
 */

#define SERIALIO_MAX_LEN_DV	128				// максимальная длинна строки
#define SERIALIO_MAX_LEN_PAR	32			// максимальная длиннас троки с параметрами
#define SERIALIO_MAX_LEN_DATA	1024		// максимальный размер буфера данных принимаемого за один раз


#define SERIAL_NPOS -1						// переменная для манипуляции строками ( позиция не определена



// например строка конфигурации линии может задаваться в следующем формате "38400 8N1 CTSRTS"
// это означает что сорость линии 38400, 8 бит данных без паритета 1 стоповый, аппаратное управление


struct StructStrCmp{
	tcflag_t	i_param;
	string		s_param;
};



/*

const  StructStrCmp StringSpeed [] =  {	{B230400,"230400"},	// определение констант скорости  для входного распознавания
												{B115200,"115200"},	// параметра скорости из строки
												{B57600,"57600"},
												{B38400,"38400"},
												{B19200,"19200"},
												{B9600,"9600"},
												{B4800,"4800"},
												{B2400,"2400"},
												{B1200,"1200"},
												{B300,"300"},
												{B50,"50"} ,
												{B0,"0"},
												{0,""}};



		// Установка параметров кадра

const   StructStrCmp StringCadr []  =  {	{CS8  ,"8N1"},				// 	8 бит данных без бита четности 1 стоп бит
											{CS8 |CSTOPB,"8N2"},			//	8 бит даннызх без бита четности 2 стоп бита
											{CS8 |  PARENB,"8E1"},			// 	8 бит данных с битом четности 1 стоп бит
											{CS8 |CSTOPB |  PARENB,"8E2"},	// 	8 бит данных с битом четности 2 стоп бита
											{CS8 | PARODD,"8O1"},			// 	8 бит данных с битом не четности 1 стоп бит
											{CS8 |CSTOPB | PARODD,"8O2"},	// 	8 бит данных битом не четности 2 стоп бита
											{CS7  ,"7N1"},
											{CS7 |CSTOPB,"7N2"},
											{CS7 |  PARENB,"7E1"},
											{CS7 |CSTOPB |  PARENB,"7E2"},
											{CS7 | PARODD,"7O1"},
											{CS7 |CSTOPB | PARODD,"7O2"},
											{CS6  ,"6N1"},
											{CS6 |CSTOPB,"6N2"},
											{CS6 |  PARENB,"6E1"},
											{CS6 |CSTOPB |  PARENB,"6E2"},
											{CS6 | PARODD,"6O1"},
											{CS6 |CSTOPB | PARODD,"6O2"},
											{CS5  ,"5N1"},
											{CS5 |CSTOPB,"5N2"},
											{CS5 |  PARENB,"5E1"},
											{CS5 |CSTOPB |  PARENB,"5E2"},
											{CS5 | PARODD,"5O1"},
											{CS5 |CSTOPB | PARODD,"5O2"},
											{0,""}};

// маска для выделения размера кадра



const  StructStrCmp StringLine [] = 	{ {CRTSCTS,"CTSRTS"},		// установить управление линией через сигналы CTS RTS
											{HUPCL, "HUPCL" },			// при закрытии устройства сбрасываются сигналы DTR RTS
											{CLOCAL, "CLOCAL"},
											{0,""}};		// неиспользуются сигналы управления линией


*/


#define SERIALIO_MASK_BIT_KADR CSIZE |CSTOPB | PARENB | PARODD


extern const  	StructStrCmp StringSpeed [];		// определение констант скорости  для входного распознавания
extern const   	StructStrCmp StringCadr [];		// Установка параметров кадра
extern const  	StructStrCmp StringLine [];		// установить управление линией через сигналы CTS RTS


#define DEF_CHAR_CONCAT		"/"					// символ разграничитель директория файл
#define DEF_LINE_NAME		"/dev/ttyUSB0"		// имя устройства по умолчанию
#define DEF_LINE_PARM		"9600 8N1 CLOCAL"	// строка для установки параметров линии по умолчанию
#define DEF_FILE_LOCK		"/var/lock/"		// директорий в которм размещается lock файл

// опеределение номеров ошибок генерируемых библиотекой SerialIO
	/*
	 * здесь должны быть определены ошибки и их описание
	 */
#define E_SERIALIO_MASK		0xF000		// переменная идентифицирует библиотеку в которой произошло исключение
#define E_SERIALIO_ERRNO	0x8000		// маска для кода ошибки
#define E_SERIALIO_BAL		0x01		// код ошибки распределение памяти
#define E_SERIALIO_STRDRV	0x02		// неправильная длинна строки имение устройства линии или имени файла блокировки
#define E_SERIALIO_STRPAR	0x03		// неверная длинна строки параметров
#define E_SERIALIO_BSPEED	0x04		// скорость линии указана не верно
#define E_SERIALIO_BCADR	0x05		// неправильный формат кадра
#define E_SERIALIO_BLINE	0x06		// неправильный формат управления линией
#define E_SERIALIO_BFLOC	0x07		// неверное имя устройства линии
#define E_SERIALIO_BLUSE	0x08		// линия используется другим процессом
#define E_SERIALIO_BOPEN	0x09		// ошибка создания файла блокировки одновременного доступа к линии
#define E_SERIALIO_BTIOS	0x0A		// ошибка во время установки структуры termios
#define E_SERIALIO_BGETL	0x0B		// ошибка во время чтения структыры termios
#define E_SERIALIO_BREST	0x0C		// ошибка при востановлении параметров последовательной линии
#define E_SERIALIO_BOPLI	0x0D		// ошибка открытия линии
#define E_SERIALIO_BCLLI	0x0E		// ошибка закрытия линии
#define E_SERIALIO_BCLBL	0x0F		// ошибка закрытия файла блокировки
#define E_SERIALIO_BDLLI	0x10		// ошибка удаления файла блокировки линии
#define E_SERIALIO_BBCPY	0x11		// ошибка копирования параметров
#define E_SERIALIO_BMAXD	0x12		// неверное значение параметра для минимальной длинны принимаемого слова
#define E_SERIALIO_BMAXBD	0x13		// неверная длинна буфера данных
#define E_SERIALIO_BGDAT	0x14		// ошибка во время чтения данных из буфера и записи в устройство
#define E_SERIALIO_BSDAT	0x15		// ошибка во время чтения данных
#define E_SERIALIO_BFLUSH	0x16		// очистки буфера приема передачи
#define E_SERIALIO_DRAIN	0x17		// возникла ошибка приожидании передачи данных ( возможно обрыв канала или устройство)
#define E_SERIALIO_SELECT	0x18		// возникла ошибка при выполнении операции select




// Возвращаемые значения используемые для опеределение возможности записи или чтения из порта исключая задержку

#define SERIAL_IO_SEL_READ	0x01										// операция чтения последовательного порта пройдет без блокировки
#define SERIAL_IO_SEL_WRITE	0x02										// Операция записи в последовательный порт пройдет без блокировки
#define SERIAL_IO_SEL_RDWR	SERIAL_IO_SEL_READ |  SERIAL_IO_SEL_WRITE	// операции чтения/записи из/в последовательного порта пройдут без блокировки
#define SERIAL_IO_SEL_EVE	0x04										// произошло событие в последовательном порту
#define SERIAL_IO_SEL_SIG	0x08										// управлени из select возвращено по сигналу
#define SERIAL_IO_SEL_ERR	0x10										// ошибка во время запроса ожидания

#define SERIAL_IO_SEL_TM	0		// управление было возвращено по таймеру или сигналу




extern tcflag_t search_str(const string&, const struct StructStrCmpg[], const unsigned int);

#define Get_Seed_line(In_str)	search_str(In_str,StringSpeed,E_SERIALIO_BSPEED)
#define Get_Cadr_line(In_str)	search_str(In_str,StringCadr,E_SERIALIO_BCADR)
#define Get_Line_line(In_str)	search_str(In_str,StringLine ,E_SERIALIO_BLINE)





class SerialIO {	// класс строится с расчетом что будет работать в потоках
private:

	struct termios *tio_save; 	// предыдущее состояние последовательного порта

	struct termios *tio_req;	// параметры линии запрашиваемые для установки

	struct termios *tio_real;	// реальное состояние  последовательного порта

	string *namedev;			// имя устройства последовательного порта до 127 символов

	string *namelockdir;		// имя директория в котором распологается файл блокировки

	int	fdline;					// номер дескриптора последовательного порта

	int fdlock;					// номер дискриптора файла блокировки одновременной работы с динией

	// для использования с select
	struct timeval ti;			// структура для используется для передачи времени select для чтения

	struct timeval to;			// структура для передачив ремени ожидания отправления в оператор

	fd_set	fds_i;				// для чтения
	fd_set	fds_o;				// для записи



//	int fdepool;				// дескриптор структуры лоя работы с событиями

//	void *lprotocol;			// указатель на реализацию протокола передачи и приема данных в/из последовательной линии



	// храним исключения которые будут генерироваться классом при возникновении ошибки
	// исключения подразделяются на исключения в которых фатальные и не подлежат востановлению
	// и исключения которые могут востановить процесс если чего то поправить или подождать или
	// просто еще раз попробовать

	// вспомогательные методы класса
	friend tcflag_t search_str(const string&, const struct StructStrCmpg[], const unsigned int);	// вспомогательная функция для выполнение
																									// распознавание параметров из линии работает
																									// через макросы
																									// Get_Seed_line(In_str)
																									// Get_Cadr_line(In_str)
																									// Get_Line_line(In_str)
	SerialIO* init_SerialIO(const string&, const string&, const string&) throw (ErTODO);	// метод конструктор
																					// в качестве параметров передается
																					// имя последоваетльного устройства первый параметр
																					// строка параметров открытия линии второй параметр
																					// строка имени файла блокировки одновременного доступа





public:
	SerialIO() throw (ErTODO);								// конструктор по умолчанию с именем по умолчанию /dev/ttyS0
															// при ощибке генерирует исключение с классом ошибки EErrorTODO

	SerialIO(const string&) throw(ErTODO);					// конструктор, в качестве параметра име устройства последовательной
															// линии, при ошибке генерирует исключение с классом ошибки ErrorTODO

	SerialIO(	const string&,								// имя последовательного устройства первый параметр,
				const string&)throw (ErTODO);				// строка параметрооткрытия линии второй параметр
															// при ошибке генерирует исключение с классом ошибки ErrorTODO


	SerialIO(	const string&,								// имя последоваетльного устройства первый параметр
				const string&,								// строка параметров открытия линии второй параметр
				const string&) throw (ErTODO);				// строка имени файла блокировки одновременного доступа
															// при ошибке генерируется исключение с классом ошибки ErrorTODO




	SerialIO(	const string&,								// имя последовательного устройства линии
				const string&,								// представленная в символьном виде строка параметров
				const string&, 								// строка с именем файла блокировки
				int ,										// идентификатор структуры epoll
				void*) throw (ErTODO);						// указатель на реализацию класса протокола обмена по последовательной линии
															// при ошибке генерируется исключение с классом ошибки ErrorTODO

		// данный калс использует следующие методы из класса lprotocol
		// lprotocol->GetToLine()	-- прочитать данные которые необходимо передать в линию
		// lprotocol->PutFromLine()	-- перочичтаь данные которые небоходимо передать в протокол
		// Данный класс записи в линию предоставлят следующие методы для внешнего управляния повдеденим класса
		// line->FlushToline()		-- очистка буферов передачив линию
		// line->FlushFromLine()	-- очиска буферов приема из линии
		// установить или сбросить событие готовности передать в линию
		// установить или сбросить событие наличия данных прочитанных из линии





	~SerialIO()throw(ErTODO);								// деструктор при ошибке генерируется исключение с классом
															// ошибки ErrorTODO


void SetLineParam(const string&) throw (ErTODO);			// устанавливает параметры линии согласно переданного параметраметрам
															// при ошибке генерируется исключение с классом ошибки ErrorTODO
															// возвращает строку реально установленных параметров
void SetLineParam(const struct termios&) throw (ErTODO);	// устанавливает параметры линии согласно структуре

void ReplaseParam() throw (ErTODO);							// переустанавливает параметры линии согласно внутренней структуре

struct termios* GetLineParam(struct termios&) throw (ErTODO);// Прочитать установленные параметры линии при ошибке генерирует исключение
															// с классом ошибки ErrorTODO возращащет строку параметров реально установленных
															//	для данной линии


int IsSet() throw (ErTODO);									// возвращает 0 если запрашиваемые и установленные параметры
															// идентичны

int SetLen(int) throw (ErTODO);								// установить минимальную длинну принимаемого слова в байтах
															// для возможной отсечки по сигналу
															// возвращает реальноустановленную величину

int PutB(unsigned char*,size_t ) throw (ErTODO);			// передать количество байт второй параметр в порт
															// первый параметр указатель на буфер
															// второй указатель на длинну буфера

int GetB(unsigned char*, size_t) throw (ErTODO);			// прочитать данные из порта
															// указатель на буфер приема
															// второй парамет указатель на его длинну
															// возвращает число прочитанных байт

void Flush() throw(ErTODO);									// очистить входную и выходную очередь

void TcDrain() throw(ErTODO);								// ожидание передачи

int GetWaitRsv(int Nbyte);									// Вычисление задержки передачи или приема Nbate в мкросекундах

int GetFd();												// получить дескриптор линии последовательного порта
															// возвращает номер дескриптора открытого последовательного порта

int SSelectPut(int) throw(ErTODO);							// дождаться возможности передать данные
															// передается число микросекунд для ожидания возможности передать данные
															// возвращает:
/*
#define SERIAL_IO_SEL_READ	0x01										// операция чтения последовательного порта пройдет без блокировки
#define SERIAL_IO_SEL_WRITE	0x02										// Операция записи в последовательный порт пройдет без блокировки
#define SERIAL_IO_SEL_RDWR	SERIAL_IO_SEL_READ |  SERIAL_IO_SEL_WRITE	// операции чтения/записи из/в последовательного порта пройдут без блокировки
#define SERIAL_IO_SEL_EVE	0x04										// произошло событие в последовательном порту
#define SERIAL_IO_SEL_SIG	0x08										// управлени из select возвращено по сигналу
#define SERIAL_IO_SEL_ERR	0x10										// ошибка во время запроса ожидания
 *
 */


int SSelectGet(int) throw(ErTODO) ;							// данные для приема доступны
															// передаетсяпараметр число микросекундля для ожидания поступления данных
															// возвращает:
/*
#define SERIAL_IO_SEL_READ	0x01										// операция чтения последовательного порта пройдет без блокировки
#define SERIAL_IO_SEL_WRITE	0x02										// Операция записи в последовательный порт пройдет без блокировки
#define SERIAL_IO_SEL_RDWR	SERIAL_IO_SEL_READ |  SERIAL_IO_SEL_WRITE	// операции чтения/записи из/в последовательного порта пройдут без блокировки
#define SERIAL_IO_SEL_EVE	0x04										// произошло событие в последовательном порту
#define SERIAL_IO_SEL_SIG	0x08										// управлени из select возвращено по сигналу
#define SERIAL_IO_SEL_ERR	0x10										// ошибка во время запроса ожидания
 *
 */


};




} /* end namespace sega */


#endif /* SRC_MON_CLIENT_DRV_SERIAL_H_ */
