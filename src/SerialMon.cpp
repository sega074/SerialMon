//============================================================================
// Name        : SerialMon.cpp
// Author      : Gordeychuk
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "Serial.h"
#include "ErrorTODO.h"
using namespace std;


using namespace std;

using namespace sega;



void printErSerial(ErTODO *er){
	switch(er->get_err() & 0xff){


	case E_SERIALIO_BAL:			//	0x01		// код ошибки распределение памяти
		cout << " ошибка распределение памяти " << endl;
		break;
	case E_SERIALIO_STRDRV:			//	0x02		// неправильная длинна строки имение устройства линии или имени файла блокировки
		cout << " неправильная длинна строки имение устройства линии или имени файла блокировки " << endl;
				break;
	case E_SERIALIO_STRPAR:			//	0x03		// неверная длинна строки параметров
		cout << " неверная длинна строки параметров " << endl;
				break;
	case E_SERIALIO_BSPEED:			//	0x04		// скорость линии указана не верно
		cout << " скорость линии указана не верно " << endl;
				break;
	case E_SERIALIO_BCADR:			//	0x05		// неправильный формат кадра
		cout << " неправильный формат кадра " << endl;
				break;
	case E_SERIALIO_BLINE:			//	0x06		// неправильный формат управления линией
		cout << " неправильный формат управления линией " << endl;
				break;
	case E_SERIALIO_BFLOC:			//	0x07		// неверное имя устройства линии
		cout << " неверное имя устройства линии " << endl;
				break;
	case E_SERIALIO_BLUSE:			//	0x08		// линия используется другим процессом
		cout << " линия используется другим процессом " << endl;
				break;
	case E_SERIALIO_BOPEN:			//	0x09		// ошибка создания файла блокировки одновременного доступа к линии
		cout << " ошибка создания файла блокировки одновременного доступа к линии " << endl;
				break;
	case E_SERIALIO_BTIOS:			//	0x0A		// ошибка во время установки структуры termios
		cout << " ошибка во время установки структуры termios " << endl;
				break;
	case E_SERIALIO_BGETL:			//	0x0B		// ошибка во время чтения структыры termios
		cout << " ошибка во время чтения структыры termios " << endl;
				break;
	case E_SERIALIO_BREST:			//	0x0C		// ошибка при востановлении параметров последовательной линии
		cout << " ошибка при востановлении параметров последовательной линии " << endl;
				break;
	case E_SERIALIO_BOPLI:			//	0x0D		// ошибка открытия линии
		cout << " ошибка открытия линии " << endl;
				break;
	case E_SERIALIO_BCLLI:			//	0x0E		// ошибка закрытия линии
		cout << " ошибка закрытия линии " << endl;
				break;
	case E_SERIALIO_BCLBL:			//	0x0F		// ошибка закрытия файла блокировки
		cout << " ошибка закрытия файла блокировки " << endl;
				break;
	case E_SERIALIO_BDLLI:			//	0x10		// ошибка удаления файла блокировки линии
		cout << " ошибка удаления файла блокировки линии " << endl;
				break;
	case E_SERIALIO_BBCPY:			//	0x11		// ошибка копирования параметров
		cout << " ошибка копирования параметров " << endl;
				break;
	case E_SERIALIO_BMAXD:			//	0x12		// неверное значение параметра для минимальной длинны принимаемого слова
		cout << " неверное значение параметра для минимальной длинны принимаемого слова " << endl;
				break;
	case E_SERIALIO_BMAXBD:			//	0x13		// неверная длинна буфера данных
		cout << " неверная длинна буфера данных " << endl;
				break;
	case E_SERIALIO_BGDAT:			//	0x14		// ошибка во время чтения данных из буфера и записи в устройство
		cout << " ошибка во время чтения данных из буфера и записи в устройство " << endl;
				break;
	case E_SERIALIO_BSDAT:			//	0x15		// ошибка во время записи данных в буфер
		cout << " ошибка во время записи данных в буфер " << endl;
				break;
	case E_SERIALIO_BFLUSH:			//	0x16		// очистки буфера приема передачи
		cout << " очистки буфера приема передачи " << endl;
				break;
	case E_SERIALIO_DRAIN:			//	0x17		// возникла ошибка приожидании передачи данных ( возможно обрыв канала или устройство)
		cout << " возникла ошибка приожидании передачи данных ( возможно обрыв канала или устройство) " << endl;
				break;
	default:
		cout << " неизвестно " << endl;
				break;

	}
}

// функция вывода
/*
 * os - поток вывода
 * first - строка которая выводится перой для идентификации вфвода
 * bf - буфер над которым производим вывод
 * len - длинна вывода
 *
 */

#define NO_PRINT_SIMBOOL	0x20	// символы котрые не печатаем

void outdate(ofstream *os, string first, unsigned char *bf, int len){

	struct timeval tmvl;
	struct timezone tmzn;
	char tmpbf[32];

	bzero(tmpbf,32);

	gettimeofday(&tmvl,&tmzn);

	(*os) << first << " ";

	ctime_r(&(tmvl.tv_sec),tmpbf);
	if (memchr(tmpbf,'\n',32) != NULL)  *((char*)(memchr(tmpbf,'\n',32))) = 0 ;

	(*os) << tmpbf << " " << tmvl.tv_usec  << " ";


	for (int i = 0 ; i < len ; i ++){

		(*os) << "(" << (unsigned int ) bf[i] << ")";
		if (bf[i] >= NO_PRINT_SIMBOOL){
			(*os) << bf[i];
		}
		(*os) << " ";
	}

	(*os) << "\n";
	(*os).flush();
}


int FlEnd = 1;

typedef struct {
	SerialIO	*serIn;
	SerialIO	*serOut;
	string		ident;
	ofstream	*of;
} Vpar;


// поток ввода вывода

void* InOut(void* In){

	unsigned char * bfret = NULL;				// буфер для приема и передачи данных
	int  ret;
	int  retlen;

	Vpar *vp;

	vp = (Vpar*)In;

	try {
		bfret = new unsigned char[SERIALIO_MAX_LEN_DATA];
	} catch (bad_alloc &e){
		FlEnd = 0;
		cout << "Bad allovate memeory of InOutPO  "  << endl;

	}

	while (FlEnd == 1){

		// очистить буфер
		try{

			do{		// ждать готовности последовтального устройства

			}while(!(vp->serIn->SSelectGet(500000) == SERIAL_IO_SEL_READ) && (FlEnd == 1));

			if (FlEnd == 0)
				continue;

			bzero ((char*)bfret, SERIALIO_MAX_LEN_DATA);


			retlen = vp->serIn->GetB(bfret, SERIALIO_MAX_LEN_DATA);					// прочитать данные с последовательного устройства


		} catch (ErTODO &er_todo) {

			cout << "Read from serial (ErTODO) err(hex) = " << hex << er_todo.get_err() << " line = " << dec << er_todo.get_line() << endl;


		}


		outdate (vp->of, vp->ident, bfret, retlen);// сохранить данные в файл


		try {


			do{		// ждать готовности последовтального устройства для передачи

			}while(!(vp->serOut->SSelectPut(500000) == SERIAL_IO_SEL_WRITE) && (FlEnd == 1));

			if (FlEnd == 0)
				continue;


			for (int w = 0; w < retlen; w+=ret, retlen -= w){
				ret = vp->serOut->PutB(bfret + w, retlen  );					// передать данные в выходное последовательное устройство
			}


		} catch (ErTODO &er_todo) {
			cout << "Send to Serial  (ErTODO) err(hex) = " << hex << er_todo.get_err() << " line = " << dec << er_todo.get_line() << endl;
		}
	}

	return NULL;
}



int main() {


	SerialIO	*serIOPO;

	SerialIO	*serIOHD;



	Vpar *ioPO;
	Vpar *ioHD;

	pthread_t		inPOwrHD;				// читает ПО передает в оборудование

	pthread_t		inHDwrPO;				// читает данные из оборудования передает в ПО





	try {

		serIOPO = new SerialIO("/dev/ttyUSB0","9600 8N1 CLOCAL", ".");
	} catch (ErTODO &e) {
		printErSerial(&e);


		throw ErTODO(e);

	}


	try {

		serIOHD = new SerialIO("/dev/ttyUSB1","9600 8N1 CLOCAL", ".");
	} catch (ErTODO &e) {
		printErSerial(&e);

		delete serIOPO;
		throw ErTODO(e);

	}

	ofstream outfile ("./InputOutputSer.txt",ios::app);

	ioPO = new Vpar;
	ioHD = new Vpar;


	ioPO->ident = string("InPr");					// поток читает данные от програмного обеспечения и передает в оборудование
	ioPO->of = &outfile;
	ioPO->serIn = serIOPO;
	ioPO->serOut = serIOHD;

	ioHD->ident = string("InHD");					// поток читает данные от оборудование и передает програмному обеспечению
	ioHD->of = &outfile;
	ioHD->serIn = serIOHD;
	ioHD->serOut = serIOPO;


	// создать потоки

	if (pthread_create(&inPOwrHD, NULL,InOut, ioHD) != 0){
		FlEnd = 0;
	}

	if (pthread_create(&inHDwrPO, NULL,InOut, ioPO ) != 0){
			FlEnd = 0;
	}


	while (FlEnd == 1){
		string instr;
		cout << " введите end для завершения : " ;
		cin >> instr;

		if (instr.compare("end")== 0){
			FlEnd = 0;
		}

		continue;

	}

	pthread_join(inPOwrHD, NULL);
	pthread_join(inHDwrPO,NULL);
	delete serIOPO;
	delete serIOHD;

	delete ioPO;
	delete ioHD;

	outfile.close();

	cout << "End SerialMon " << endl;

}
