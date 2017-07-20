/*
 * Serial.cpp
 *
 *  Created on: 14 сент. 2015 г.
 *      Author: sega
 */


#include <stdio.h>
#include <stdlib.h>

#include <iostream>			// для отладки


#include "Serial.h"

using namespace std;

namespace sega{


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// определение массивов данны для конфигурации
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#define SERIALIO_MASK_BIT_KADR CSIZE |CSTOPB | PARENB | PARODD

const  StructStrCmp StringLine [] = 	{ {CRTSCTS,"CTSRTS"},		// установить управление линией через сигналы CTS RTS
											{HUPCL, "HUPCL" },			// при закрытии устройства сбрасываются сигналы DTR RTS
											{CLOCAL, "CLOCAL"},
											{0,""}};		// неиспользуются сигналы управления линией















/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// вспомогательные методы класса и функции
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

tcflag_t search_str(const string &stri, const struct StructStrCmp Scmp[], const unsigned int Er){ // Поиск параметров в строке
	tcflag_t tf_t= 0;
	int fl = 1;
	for (int i = 0 ;Scmp[i].i_param !=0 && Scmp[i].s_param.length() != 0 ;i++){

			if (stri.find(Scmp[i].s_param,0) != (size_t)SERIAL_NPOS) {	// совпадение параметра в строке , установим параметр
				tf_t = tf_t | Scmp[i].i_param;
				fl=0;
				break;
			}

		}
	if (fl){
		throw ER_TODO(E_SERIALIO_MASK|Er);
	}
	return tf_t;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Реализации методов класа
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SerialIO* SerialIO::init_SerialIO(const string &s_line, const string &s_param, const string &s_lock) throw (ErTODO){	// конструктор, в качестве параметров передается
																					// имя последоваетльного устройства первый параметр
																					// строка параметров открытия линии второй параметр
																					// строка имени файла блокировки одновременного доступа
	try {


// начальные определения

			this->fdline = -1;							// признак не открытого устройства линии

			this->fdlock = -1;							// признак нет открытого файла для блокировки одновременного доступа

			this->tio_save = NULL;						// нет сохраненного предыдущего состояния

			this->tio_req = NULL;						// нет сохраненного запрашиваемого сотояния

			this->tio_real = NULL;						// нет установленных состояний линий

			if(s_line.length() > SERIALIO_MAX_LEN_DV || s_line.length() < 1 ){
				throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_STRDRV	);			// неверная длинна строки имени устройства линии
			} else {
				this->namedev  = new string(s_line);							// устновить имя устройства
			}

			if(s_lock.length() > SERIALIO_MAX_LEN_DV || s_lock.length() < 1 ){

				throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_STRDRV);			// неверная длинна строки имени каталога файла  блокировки
																				// одновременного доступа

			} else {

				this->namelockdir  = new string(s_lock);						// устновить имя каталога для файла блокировки одновременного доступа

				size_t szt = this->namedev->find_last_of(string(DEF_CHAR_CONCAT),this->namedev->length() );	// ищем позицию последнего символа разделителя "/"
																						// string(DEF_CHAR_CONCAT)

				if ((szt !=0) && ((szt+1) < this->namedev->length())){			// индекс с которого начинается имя устройства линии должен быть меньше
					//string ss = 		this->namedev->substr(szt+1,this->namedev->length());											// длинны строки

//					int i1 = this->namelockdir->length();
//					int i2 = this->namelockdir->find_last_of(string(DEF_CHAR_CONCAT),this->namelockdir->length());

					if ((this->namelockdir->find_last_of(string(DEF_CHAR_CONCAT),this->namelockdir->length()) +1 ) < (this->namelockdir->length())){
						this->namelockdir->append(string(DEF_CHAR_CONCAT));
					}

					this->namelockdir->append(this->namedev->substr(szt+1,this->namedev->length()));	// формируем имя файла для блокировки доступа к линии


				} else {
																			// не можем выделить имени устройства для создание файла блокировки
																			// по этому генерируем исключение
						throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_BFLOC);	// здесь генерируется исключение если только неверно определены имена
																			// файлов для линии в файле SerialIO.h
				}
			}


			if(s_param.length() > SERIALIO_MAX_LEN_PAR || s_param.length() < 1 ){

					throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_STRPAR	);		// неверная длинна строки инициализации линии

			} else {

				tio_save = new struct termios;									// выделение памяти для сохранения данных линии
				bzero((void*)tio_save,sizeof(struct termios));					// обнулить выделенную под структуру память

				tio_req = new struct termios;									// выделение памяти для сохраненияданных текущих установок линии
				bzero((void*) tio_req,sizeof(struct termios));					// обнулить выделенную под структуру память

				if (cfsetspeed(tio_req,Get_Seed_line(s_param) ))
					throw ER_TODO( E_SERIALIO_MASK | E_SERIALIO_BTIOS);			// ошибка при установлении параметра скорости в termios

					tio_req->c_cflag |=Get_Cadr_line(s_param);					// установить параметры кадара
					tio_req->c_cflag |= Get_Line_line(s_param);				// установить параметры управления
					tio_req->c_cc[VMIN] = 1;									// устанавливаем что возвращаем даже 1 символ
					tio_req->c_cc[VTIME]= 0 ;									// неждем поступления других символов а сразу возращаем

				tio_real = new struct termios;									// память для запроса параметров линии
				bzero((void*) tio_real,sizeof(struct termios));
			}


// создать файл блокировки

				this->fdlock = open(this->namelockdir->c_str(),O_RDWR| O_CREAT| O_EXCL, O_EXCL|S_IWUSR);

				 if (this->fdlock < 0 && errno ==  EEXIST){	// файл блокировки существует те другой процес использует данное устройство
					  throw ER_TODO (E_SERIALIO_MASK|E_SERIALIO_BLUSE);


				} else if (this->fdlock < 0){	// какято другая ошибка создания файла блокировки
					throw ER_TODO(E_SERIALIO_MASK|E_SERIALIO_BOPEN);

				}

// теперь открыть линию и установить ее параметры

				 fdline = open(this->namedev->c_str(),O_RDWR |O_NOCTTY|  O_NONBLOCK); 	// открыть для чтения, записи, не контролирующее устр,
				 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 // не блокировать

				 //fdline = open(this->namedev->c_str(),O_RDWR |O_NOCTTY);

				 if (fdline < 0){
					 close(this->fdlock);
					 unlink(this->namelockdir->c_str());
					 ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BOPLI);

				 }



	} catch (bad_alloc &bal){

		// освободить память выделенную переменных структур и классов

		throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_BAL	); 	// Сформировать код исключения как побитовое объединение
															// маски с кодом библиотеки и код ошибки
	} catch (ErTODO &er_todo){

		// сдесь освобождаем ресурсы и повтороно генерируем исключение

		try{										// освобождаем все занятые ресурсы
			//delete namedev;
			namedev->~basic_string();
		} catch(runtime_error &rt){}

		try{
			delete namelockdir;
		} catch (runtime_error &rt){}

		try {
			delete tio_save;
		} catch (runtime_error &rt){}

		try {
			delete tio_req;
		} catch (runtime_error &rt){}

		try {
			delete tio_real;
		} catch (runtime_error &rt) {}
		throw;										// повторно генерируем исклюыение
	}

	if (tcgetattr(this->fdline,this->tio_save) != 0){				// сохранить параметры линии
																	// для востановления при закрытии
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);		// не фатальная ( ресурс не освобождаем)
	}


	if (tcsetattr(this->fdline, TCSANOW , this->tio_req) != 0){ 	// установка параметров линии
		throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BTIOS	);			// не фатальная ( ресурс не освобождаем)
	}

	if (tcgetattr(this->fdline,this->tio_real) != 0){				// проверка на правильность установки проводится
																	// другим методом
		throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}


	return this;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Реализации методов класа
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SerialIO::SerialIO() throw (ErTODO){			// создаем класс, и создаем файл блокировки линии
												// если не можем создать файл блокировки то другой процесс использует данную последовательную
												// линию

	init_SerialIO(DEF_LINE_NAME,DEF_LINE_PARM,DEF_FILE_LOCK);

}




SerialIO::~SerialIO()throw(ErTODO){	// вуструктор

	if (this->fdline > 0){			// востановить первоначальные значения для линии



			if (tcsetattr(this->fdline,TCSANOW,this->tio_save)  != 0 ){ // сгенерируем исключение
				ER_TODO( E_SERIALIO_MASK |E_SERIALIO_BREST);
			}
			close (this->fdline);
	}

	if(this->fdlock > 0){
		close (this->fdlock);

		this->fdlock = -1;

		unlink(this->namelockdir->c_str());
	}



			try{										// освобождаем все занятые ресурсы

				//delete namedev;
				namedev->~basic_string();
			} catch(runtime_error &rt){}

			try{
				//delete namelockdir;
				namelockdir->~basic_string();
			} catch (runtime_error &rt){}

			try {
				delete tio_save;
			} catch (runtime_error &rt){}

			try {
							delete tio_req;
						} catch (runtime_error &rt){}

			try {
				delete tio_real;
			} catch (runtime_error &rt) {}


}






SerialIO::SerialIO(const string& strdv) throw(ErTODO){				// конструктор, в качестве параметра имя устройства последовательной
																		// линии, при ошибке генерирует исключение с классом ошибки ErrorTODO

	init_SerialIO(strdv,DEF_LINE_PARM,DEF_FILE_LOCK);

}




SerialIO::SerialIO(const string &strdv,const string &strpar)throw (ErTODO){	// конструктор, в качестве параметров передается
																	// имя последовательного устройства первый параметр,
																	// строка параметрооткрытия линии второй параметр
																	// при ошибке генерирует исключение с классом ошибки ErrorTODO
	init_SerialIO(strdv,strpar,DEF_FILE_LOCK);

}




SerialIO::SerialIO(const string &strdrv, const string &strpar, const string &strlock) throw (ErTODO){ // конструктор, в качестве параметров передается
																		// имя последоваетльного устройства первый параметр
																		// строка параметров открытия линии второй параметр
																		// строка имени файла блокировки одновременного доступа
	init_SerialIO(strdrv,strpar,strlock);

}





void SerialIO::SetLineParam(const string &s_param) throw (ErTODO){		// устанавливает параметры линии по строке
															// при ошибке генерируется исключение с классом ошибки ErrorTODO
															// возвращает строку реально установленных параметров

	if(s_param.length() > SERIALIO_MAX_LEN_PAR || s_param.length() < 1 ){

			throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_STRPAR	);		// неверная длинна строки инициализации линии

	} else {



		bzero((void*) tio_req,sizeof(struct termios));					// обнулить выделенную под структуру память

		if (cfsetspeed(tio_req,Get_Seed_line(s_param) ))
			throw ER_TODO( E_SERIALIO_MASK | E_SERIALIO_BTIOS);			// ошибка при установлении параметра скорости в termios

			tio_req->c_cflag |=Get_Cadr_line(s_param);					// установить параметры кадара
			tio_req->c_cflag |= Get_Line_line(s_param);					// установить параметры управления
			tio_req->c_cc[VMIN] = 1;									// устанавливаем что возвращаем даже 1 символ
			tio_req->c_cc[VTIME]= 0 ;									// неждем поступления других символов а сразу
																		// возращаем
	}



	if (tcsetattr(this->fdline, TCSANOW , this->tio_req) != 0){ 	// установка параметров линии
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BTIOS	);			// не фатальная ( ресурс не освобождаем)
	}

	bzero((void*) tio_real,sizeof(struct termios));

	if (tcgetattr(this->fdline,this->tio_real) != 0){				// проверка на правильность установки проводится
																		// другим методом
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}



}
void SerialIO::SetLineParam(const struct termios &s_param) throw (ErTODO){			//

	try {

		bcopy(&s_param,this->tio_req,sizeof(struct termios));	// за одно и проверяем правильность параметров линии

	} catch(...){
		throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BBCPY);
	}

	if (tcsetattr(this->fdline, TCSANOW , this->tio_req) != 0){ 	// установка параметров линии
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BTIOS	);			// не фатальная ( ресурс не освобождаем)
	}

	bzero((void*) tio_real,sizeof(struct termios));

	if (tcgetattr(this->fdline,this->tio_real) != 0){					// прочитать что установили, проверка на правильность установки проводится
																		// другим методом
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}


}


void SerialIO::ReplaseParam() throw (ErTODO){

	if (tcsetattr(this->fdline, TCSANOW , this->tio_req) != 0){ 		// установка параметров линии
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BTIOS	);			// не фатальная ( ресурс не освобождаем)
	}

	bzero((void*) tio_real,sizeof(struct termios));

	if (tcgetattr(this->fdline,this->tio_real) != 0){					// перочитать что установили, проверка на правильность установки проводится
																		// другим методом
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}


}


struct termios* SerialIO::GetLineParam(struct termios &tio) throw (ErTODO){   	// Прочитать установленные параметры линии при ошибке генерирует исключение
																				// с классом ошибки ErrorTODO возращащет строку параметров реально установленных
																				//	для данной линии

	if (tcgetattr(this->fdline,this->tio_real) != 0){					// чтение установленных параметров линии
																		//
			throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}

	bcopy (this->tio_real,&tio,sizeof(struct termios));

	return (struct termios*) &tio;
}


int SerialIO::IsSet() throw(ErTODO){											// возвращает 0 если запрашиваемые и установленные параметры
																				// идентичны

	return bcmp(this->tio_req,this->tio_real, sizeof(struct termios));


}


int SerialIO::SetLen(int len_min) throw (ErTODO){				// установить длинну принимаемого слова в байтах
																// для возможной отсечки по сигналу
																// возвращает реально установленное число

	if ( len_min > 0 && len_min < SERIALIO_MAX_LEN_DATA){

	tio_req->c_cc[VMIN] = len_min;								// по установленному числу необходимо вычислить задержку
																// и установить задержку ( сделать потом)
	} else {
		throw ER_TODO (E_SERIALIO_MASK| E_SERIALIO_BMAXD);			// не верный параметр минимальной длинны принимамых данных
	}

	if (tcsetattr(this->fdline, TCSANOW , this->tio_req) != 0){ 	// установка параметров линии
				throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BTIOS	);	// не фатальная ( ресурс не освобождаем)
	}

	bzero((void*) tio_real,sizeof(struct termios));

	if (tcgetattr(this->fdline,this->tio_real) != 0){						// чтение установленых параетров, проверка на правильность установки проводится
																			// другим методом
				throw ER_TODO(E_SERIALIO_MASK | E_SERIALIO_BGETL	);			// не фатальная ( ресурс не освобождаем)
	}

	return this->tio_real->c_cc[VMIN];

}

int SerialIO::PutB(unsigned char *bf,size_t bf_len ) throw (ErTODO){			// передать количество байт второй параметр в порт
															// первый параметр указатель на буфер
															// второй указатель на длинну буфера

	if (bf_len > SERIALIO_MAX_LEN_DATA){				// проверить допустимую длинну передаваемых данных
		throw ER_TODO( E_SERIALIO_MASK | E_SERIALIO_BMAXBD);
	}



	int retwr;				// переменная для возврата

	retwr = write (this->fdline,bf,bf_len);				// помним что запись не блокируемая
	if (retwr < 0){

		throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_BGDAT);
	}


	return retwr;			// вернуть число записанных данных

}

int SerialIO::GetB(unsigned char *bf, size_t bf_len) throw (ErTODO){				// прочитать данные из порта
															// указатель на буфер приема
															// второй парамет указатель на его длинну
	if (bf_len > SERIALIO_MAX_LEN_DATA){					// проверить допустимую длинну буфера для приема данных
		throw ER_TODO( E_SERIALIO_MASK | E_SERIALIO_BMAXBD);
	}

	int retrd = 0;				// переменная для возврата


	//cout << "GetB bf = " << hex << (unsigned long) bf << " len = " << dec << (int) bf_len << endl;

	retrd = read(this->fdline,bf,bf_len);				// помним что запись не блокируемая

	//cout << "GetB retrd = " <<  retrd << endl;

	if (retrd >=0){

		return retrd;		// венуть число прочитанных байт

	} else if (retrd < 0 && ((EINTR == errno) || EAGAIN  == errno ) ){	// не связанна с отсутствием данных
			retrd = 0;
	} else {
			throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_BSDAT);
	}
	return retrd;
}



void SerialIO::Flush() throw(ErTODO){		// очистить входную и выходную очередь

	if (tcflush(fdline,TCIOFLUSH) == -1){
		throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_BFLUSH);
	}

}

void SerialIO::TcDrain() throw (ErTODO){ 	// ожидание передачи данных

	if(tcdrain(fdline) == -1){				// ожидание завершилось по какой то
											// непонятной причие возмжно получен сигнал

		throw	ER_TODO(E_SERIALIO_MASK |E_SERIALIO_DRAIN);
	}
}



int SerialIO::GetWaitRsv(int Nbyte){		// Вычисление задержки передачи или приема Nbate в мкросекундах

	double bit_sec;					// время передачи одного бита

	int bit_byte;					// размер в битах передаваемой единицы

	//прочитать скорость
	switch(cfgetispeed(tio_real)) {
		case B230400:
			bit_sec = 1000000/230400;
			break;
		case B115200:
			bit_sec = 1000000/115200;
			break;
		case B57600:
			bit_sec = 1000000/57600;
			break;
		case B38400:
			bit_sec = 1000000/38400;
			break;
		case B19200:
			bit_sec = 1000000/19200;
			break;
		case B9600:
			bit_sec = 1000000/9600;
			break;
		case B4800:
			bit_sec = 1000000/4800;
			break;
		case B2400:
			bit_sec = 1000000/2400;
			break;
		case B1200:
			bit_sec = 1000000/1200;
			break;
		case B300:
			bit_sec = 1000000/300;
			break;
		default:
			bit_sec = 0;
			break;
	}
	// прочитать число стоповых бит
	// прочитать бит четности и не четности
	// прочитать формат кадра


	switch (tio_real->c_cflag & ((tcflag_t)SERIALIO_MASK_BIT_KADR)) {

	case	CS8:					// 	8 бит данных без бита четности 1 стоп бит
		bit_byte	= 10;
		break;
	case	CS8 |CSTOPB:			//	8 бит даннызх без бита четности 2 стоп бита
		bit_byte	= 11;
		break;
	case 	CS8 |PARENB:			// 	8 бит данных с битом четности 1 стоп бит
		bit_byte	= 11;
		break;
	case	CS8 |CSTOPB |PARENB:	// 	8 бит данных с битом четности 2 стоп бита
		bit_byte	= 12;
		break;
	case	CS8 |PARODD:			// 	8 бит данных с битом не четности 1 стоп бит
		bit_byte	= 11;
		break;
	case	CS8 |CSTOPB |PARODD:	// 	8 бит данных битом не четности 2 стоп бита
		bit_byte	= 12;
		break;
	case	CS7:					// 	7 бит данных без бита четности 1 стоп бит
		bit_byte	= 9;
		break;
	case	CS7 |CSTOPB:			//	7 бит даннызх без бита четности 2 стоп бита
		bit_byte	= 10;
		break;
	case	CS7 |PARENB:			// 	7 бит данных с битом четности 1 стоп бит
		bit_byte	= 10;
		break;
	case	CS7 |CSTOPB |PARENB:	// 	7 бит данных с битом четности 2 стоп бита
		bit_byte	= 11;
		break;
	case	CS7 |PARODD:			// 	7 бит данных с битом не четности 1 стоп бит
		bit_byte	= 10;
		break;
	case	CS7 |CSTOPB |PARODD:	// 	7 бит данных битом не четности 2 стоп бита
		bit_byte	= 11;
		break;
	case	CS6:					// 	6 бит данных без бита четности 1 стоп бит
		bit_byte	= 8;
		break;
	case	CS6 |CSTOPB:			//	6 бит даннызх без бита четности 2 стоп бита
		bit_byte	= 9;
		break;
	case	CS6 |PARENB:			// 	6 бит данных с битом четности 1 стоп бит
		bit_byte	= 9;
		break;
	case	CS6 |CSTOPB |PARENB:	// 	6 бит данных с битом четности 2 стоп бита
		bit_byte	= 10;
		break;
	case	CS6 |PARODD:			// 	6 бит данных с битом не четности 1 стоп бит
		bit_byte	= 8;
		break;
	case	CS6 |CSTOPB |PARODD:	// 	6 бит данных битом не четности 2 стоп бита
		bit_byte	= 10;
		break;
	case	CS5:					// 	5 бит данных без бита четности 1 стоп бит
		bit_byte	= 7;
		break;
	case	CS5 |CSTOPB:			//	5 бит даннызх без бита четности 2 стоп бита
		bit_byte	= 8;
		break;
	case	CS5 |PARENB:			// 	5 бит данных с битом четности 1 стоп бит
		bit_byte	= 8;
		break;
	case	CS5 |CSTOPB |PARENB:	// 	5 бит данных с битом четности 2 стоп бита
		bit_byte	= 9;
		break;
	case	CS5 |PARODD:			// 	5 бит данных с битом не четности 1 стоп бит
		bit_byte	= 8;
		break;
	case	CS5 |CSTOPB |PARODD:	// 	5 бит данных битом не четности 2 стоп бита
		bit_byte	= 9;
		break;
	default:
		bit_byte = 0;
		break;
	}

	return ((unsigned int)(bit_sec * bit_byte * Nbyte)) + 1;
}


int SerialIO::GetFd(){													// получить дескриптор линии последовательного порта
																		// возвращает номер дескриптора открытого последовательного порта
	return fdline;
}


/*
 * Выполняе операцию проверки готовности данных по записи (select)
 * вкачестве входного параметра задается количиство микросекунд для ожидания
#define SERIAL_IO_SEL_READ	0x01										// операция чтения последовательного порта пройдет без блокировки
#define SERIAL_IO_SEL_WRITE	0x02										// Операция записи в последовательный порт пройдет без блокировки
#define SERIAL_IO_SEL_RDWR	SERIAL_IO_SEL_READ |  SERIAL_IO_SEL_WRITE	// операции чтения/записи из/в последовательного порта пройдут без блокировки
#define SERIAL_IO_SEL_EVE	0x04										// произошло событие в последовательном порту
#define SERIAL_IO_SEL_SIG	0x08										// управлени из select возвращено по сигналу
#define SERIAL_IO_SEL_ERR	0x10										// ошибка во время запроса ожидания
 *
 */
int SerialIO::SSelectPut(int tms)throw(ErTODO){
//	int retsl;
	to.tv_sec = (unsigned int) (tms/1000000);
	to.tv_usec = (unsigned int) (tms%1000000);


	FD_ZERO(&fds_o);
	FD_SET(fdline, &fds_o);

	 switch ( select(fdline+1,NULL, &fds_o, NULL, &ti)){
		case -1:
			switch(errno){
			case EINTR:
					return SERIAL_IO_SEL_SIG;			// возврат по сигналу
			case EBADF:
			case EINVAL:
			case ENOMEM:
			default:
				throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_SELECT);
			}
			break;
		case 0:
			break;
		default:
			if (FD_ISSET(fdline,&fds_o)){
				return SERIAL_IO_SEL_WRITE;
			}
			break;
	 }
	 return SERIAL_IO_SEL_TM;

}

/*
 * Выполняе операцию проверки готовности данных по чтению (select)
 * вкачестве входного параметра задается количиство микросекунд для ожидания
#define SERIAL_IO_SEL_READ	0x01										// операция чтения последовательного порта пройдет без блокировки
#define SERIAL_IO_SEL_WRITE	0x02										// Операция записи в последовательный порт пройдет без блокировки
#define SERIAL_IO_SEL_RDWR	SERIAL_IO_SEL_READ |  SERIAL_IO_SEL_WRITE	// операции чтения/записи из/в последовательного порта пройдут без блокировки
#define SERIAL_IO_SEL_EVE	0x04										// произошло событие в последовательном порту
#define SERIAL_IO_SEL_SIG	0x08										// управлени из select возвращено по сигналу
#define SERIAL_IO_SEL_ERR	0x10										// ошибка во время запроса ожидания
 *
 */

int SerialIO::SSelectGet(int tms) throw(ErTODO){
//	int retsl;

	ti.tv_sec = (unsigned int) (tms/1000000);;
	ti.tv_usec = (unsigned int) (tms%1000000);


	 FD_ZERO(&fds_i);
	 FD_SET(fdline, &fds_i);

	 switch ( select(fdline+1,&fds_i, NULL, NULL, &ti)){
		case -1:
			switch(errno){
			case EINTR:
					return SERIAL_IO_SEL_SIG;			// возврат по сигналу
			case EBADF:
			case EINVAL:
			case ENOMEM:
			default:
				throw ER_TODO(E_SERIALIO_MASK |E_SERIALIO_SELECT);
			}
			break;
		case 0:
			break;
		default:
			if (FD_ISSET(fdline,&fds_i)){
				return SERIAL_IO_SEL_READ;
			}
			break;
	 }
	 return SERIAL_IO_SEL_TM;
}


} // name space sega


