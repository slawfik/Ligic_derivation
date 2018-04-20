// Skusam to cez ssh!
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>

#define END_TAG 0
#define NO_END_TAG 1
#define VELKOST_BUFFRA 2
#define OUT_NUMBER 1
#define OUT_VECTOR 0

char *Data;

int and_and(char **pa_functionData,int pa_pocetDat,int* pa_zmenaFun_outputType,int pa_pocetProc)	{
	int _rankP;
	int _pocet = 0;
	int aa;
	int bb;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	for(int _i = 0;_i<(pa_pocetDat/2);_i++)	{
		aa = pa_functionData[_i][pa_zmenaFun_outputType[0]]-'0';
		bb = pa_functionData[_i][!pa_zmenaFun_outputType[0]]-'0';
		//printf("PROCESS__%d:  %d_*_!%d = %d\n",_rankP,bb,aa,(bb*!aa));		//DEEBUG VÝPIS
		pa_functionData[_i][0] = ((bb*!aa)+'0');

		if(pa_zmenaFun_outputType[1] == OUT_NUMBER) {
			if(pa_functionData[_i][0] == '1')	{
				_pocet++;
			}
		}
	}//poslat pocet a primatel ma zacat počítať!!
	if(pa_zmenaFun_outputType[1] == OUT_NUMBER) {
		pa_functionData[0][0] = _pocet;
	}
	MPI_Ssend(&pa_functionData[0][0],pa_pocetDat, MPI_CHAR,pa_pocetProc-1,4, MPI_COMM_WORLD);
}

void stopReceive(int pa_pocetProc)	{
	for(int i=1;i<pa_pocetProc-1;i++)	{
		MPI_Ssend(NULL,0, MPI_INT,i, END_TAG, MPI_COMM_WORLD);
	}
}

char **alloc_matrix(int riadky) {	/*vytvorý maticu (n*2) */
    Data = (char *)malloc(riadky*2*sizeof(char));
    char **_function_data = (char **)malloc(riadky*sizeof(char*));
    for (int i=0; i<riadky; i++)	{
        _function_data[i] = &(Data[2*i]);
	}
    return _function_data;
}

double dajPocetPremennych(FILE* pa_subor)	{
	unsigned long _pocetBytes;
	double _vysledok;
	double _citatel;
	double _menovatel;
	fseek(pa_subor, 0, SEEK_END);
	_pocetBytes = ftell(pa_subor);
	_pocetBytes--;
	_citatel = log(_pocetBytes);
	_menovatel = log((double)2);
	return (_citatel/_menovatel);
}

char dajData(unsigned long pa_premNa2,unsigned long* pa_nacitaneNULL,unsigned long* pa_nacitaneJEDN,int pa_type,FILE* pa_subor)	{
	unsigned long _polohaVsubore = 0;
	unsigned long _pom;
	if(pa_type == 0)	{
		_pom = *pa_nacitaneNULL/pa_premNa2;
		_polohaVsubore = *pa_nacitaneNULL+(_pom*pa_premNa2);
		*pa_nacitaneNULL=*pa_nacitaneNULL+1;
		//printf("---- 0----- _polohavSUBIRE %ld\n",_polohaVsubore);		//DEEBUG VÝPIS
	}	else {
		_pom = *pa_nacitaneJEDN/pa_premNa2;
		_polohaVsubore =*pa_nacitaneJEDN+((_pom+1)*pa_premNa2);
		*pa_nacitaneJEDN=*pa_nacitaneJEDN+1;
		//printf("---- 1----- _polohavSUBIRE %ld\n",_polohaVsubore);		//DEEBUG VÝPIS
	}
	fseek(pa_subor,_polohaVsubore,SEEK_SET);
	return fgetc(pa_subor);
}

int noParalel(int pa_pocetPrem, int pa_derivPodlaPrem, int pa_pom1,FILE* _subor)	{
	unsigned long _pocitadlo = 0;
	unsigned long _dlzka_vektoraF;
	unsigned long _pocetNacitanychJedn = 0;
	unsigned long _pocetNacitanychNull = 0;
	unsigned long _pom2;
	int a;
	int b;
	
	_dlzka_vektoraF = (unsigned long) pow(2,(double)pa_pocetPrem);
	printf("NO-PARALEL Počet premenných je: %d a dĺžka pravdivostného vektora je: %ld NO-PARALEL\n",pa_pocetPrem,_dlzka_vektoraF);
	printf("Derivácia f(1->0) / c%d(%d->%d) :\n",pa_derivPodlaPrem,!pa_pom1,pa_pom1);
	_pom2 = (unsigned long) pow(2,(double)pa_derivPodlaPrem);
	if(pa_pom1 == 0)	{
		while(_pocitadlo != _dlzka_vektoraF)	{
			a = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor)-'0';
			b = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor)-'0';
			printf("!%d_*_%d = %d\n",a,b,(!a*b));
			_pocitadlo = _pocitadlo+2;
		}
	} else {
		while(_pocitadlo != _dlzka_vektoraF)	{
			a = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor)-'0';
			b = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor)-'0';
			printf("%d_*_!%d = %d\n",a,b,(a*!b));
			_pocitadlo = _pocitadlo+2;
		}
	}
	fclose(_subor);
}

int prijmanieDat(int pa_pocetProc,int* pa_zmenaFun_outputType)	{
	int _velkostDat;
	int _pocitadloProc;
	int _process;
	int _end = 0;
	int _i;
	unsigned long _vysledok_NUM = 0;
	char* _vysledokDerivacie;
	MPI_Status _status;

	MPI_Bcast(pa_zmenaFun_outputType, 2, MPI_INT, 0,MPI_COMM_WORLD);
	if(pa_zmenaFun_outputType[0] == -1)	{
		/*##########################################*/
		/* Ukončenie prijímajúceho procesu s chybou */
		/*##########################################*/
		return -1;
	} else {
		_pocitadloProc = pa_pocetProc-2;
		_process = (_pocitadloProc%(pa_pocetProc-2))+1;
		MPI_Probe(_process, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
		MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
		_vysledokDerivacie = (char*) malloc(sizeof(char)*_velkostDat);
		while(_end != (pa_pocetProc-2))	{
			MPI_Recv(&_vysledokDerivacie[0], _velkostDat, MPI_CHAR, _process, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
			MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
			_i = 0;
			if(_status.MPI_TAG != END_TAG && pa_zmenaFun_outputType[1] == OUT_VECTOR)	{
				while(_i != _velkostDat/2)	{
					printf("TH Process: %d velkosť dát %d a výsledok _%c_!\n",_process,_velkostDat/2,_vysledokDerivacie[_i*2]);
					_i++;
				}
			} else if(_status.MPI_TAG != END_TAG && pa_zmenaFun_outputType[1] == OUT_NUMBER) {
				_vysledok_NUM += _vysledokDerivacie[0];
			}else {
				//printf("end++\n");		//DEEBUG VÝPIS
				_end++;	
			}
			_pocitadloProc++;
			_process = (_pocitadloProc%(pa_pocetProc-2))+1;
		}
		if(pa_zmenaFun_outputType[1] == OUT_NUMBER) {
			printf("Počet jednotiek vo výsledku derivácie _%ld_!\n",_vysledok_NUM);
		}
		printf("Vlakno skoncilo\n");
		free(_vysledokDerivacie);
	}
}

void error(int pa_errorNum)	{
	switch(pa_errorNum)	{
		case 1:
			;
			break;
		case 2:
			;
	}
}

int main(int argc, char** argv) {
/*---------DEFINE VARIABLES FOR EACH PROCESS------------*/
	int _rankP;
	int _pocetProc;
	int _zmenaFun_outputType[2];
	char** _function_data;
	double _pocetPrem;
/*---------END DEFINE VARIABLES FOR EACH PROCESS------------*/

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetProc);
	
/*_________________DEFINE ROOT PROCESS_________________*/
	if(_rankP == 0)	{
				
	  /*DEFINE VARIABLES FOR ROOT PROCESS*/
		pthread_t _vlaknoPrePrijmanie;
	  	unsigned long _dlzka_vektoraF;
		unsigned long _pocetNacitanychNull;
		unsigned long _pocetNacitanychJedn;
		unsigned long _pocetNULLDatPreProcess;
		unsigned long _pocetDatPreProcess;
		unsigned long _premennaNaDruhu;
		unsigned long _pocitadlo = 0;
		FILE* _subor;
		int _process = (_pocetProc-2);
		int ii;
		int _zvysok;
		int _derivPodlaPrem;
		int _pom;
	  /*END DEFINE VARIABLES FOR ROOT PROCESS*/
	  
	  /*----INICIALIZÁCIA PREMENNÝCH----*/
		_derivPodlaPrem = atoi(argv[2]);			//Zistím premennú podla ktorej budeme derivovať
		_zmenaFun_outputType[0] = atoi(argv[3]);	//na aku hodnotu sledujeme zmenu funkcie
		_subor = fopen(argv[1],"r");
		if(argv[4] != NULL)	{						//ZYSTIM TYP VÝSTUPU
			_zmenaFun_outputType[1] = OUT_NUMBER;
		} else {
			_zmenaFun_outputType[1] = OUT_VECTOR;
		}
		if(_subor == NULL)	{	
			printf("Nepodarilo sa otvoriť súbor %s\n",argv[1]);
			_zmenaFun_outputType[0] = -1;
			MPI_Bcast(&_zmenaFun_outputType[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		}
		_pocetPrem = dajPocetPremennych(_subor);
		_dlzka_vektoraF = (unsigned long) pow(2,_pocetPrem);
	  /*----END INIT PREMENNÝCH----------*/
		
		if(_pocetProc == 1)	{  /*IF NO paralel*/
			noParalel(_pocetPrem,_derivPodlaPrem,_zmenaFun_outputType[0],_subor);
			MPI_Finalize();
			return 0;
		
		} else if((_pocetProc-2) > _dlzka_vektoraF/2 | _pocetProc == 2) { /* ERROR INPUT BAD PROCESS NUMBER */
			_zmenaFun_outputType[0] = -1;
			printf("Error výpočet môže byť rozdelený maximálne do %d procesov a nesmú sa použiť 2 procesy!\n",(_dlzka_vektoraF/2)+1);
			MPI_Bcast(&_zmenaFun_outputType[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		
		} else if((double)_derivPodlaPrem >= _pocetPrem ) { /* ERROR INPUT BAD VARIABLE FOR DERIVATION */
			_zmenaFun_outputType[0] = -1;
			printf("Error premená musí byť menšia ako počet všetkých premenných!! Počet premenných je %.1f\n",_pocetPrem);
			MPI_Bcast(&_zmenaFun_outputType[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		
		}else {	/*Paralelné spracovanie*/
			_function_data = alloc_matrix(VELKOST_BUFFRA);
			//MPI_Buffer_attach(b,(VELKOST_BUFFRA*2)+MPI_BSEND_OVERHEAD);
			printf("Počet premenných je: %.1f a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
			printf("Derivácia f(1->0) / c%d(%d->%d) :\n",_derivPodlaPrem,!_zmenaFun_outputType[0],_zmenaFun_outputType[0]);
			MPI_Bcast(&_zmenaFun_outputType[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			
			_pocetDatPreProcess = (_dlzka_vektoraF/2)/(_pocetProc-2);
			_zvysok = (_dlzka_vektoraF/2)%(_pocetProc-2);
			_premennaNaDruhu = pow(2,(double)_derivPodlaPrem); /*premenna podla ktorej chceme 2^derivovať*/
			_pocetNacitanychJedn = 0;
			_pocetNacitanychNull = 0;
			if(_pocetDatPreProcess < VELKOST_BUFFRA)	{
				_pocetDatPreProcess++;
				_pom = -1;
			}
		
			while(_pocitadlo != _dlzka_vektoraF)	{		/* DATA PARSING */
				if(_process == ((_pocetProc-2)*20))	{
					_process = (_pocetProc-2);
				}
				if(_pom == -1)	{
					if(_zvysok == 0 && _pom != 5)	{
						_pocetDatPreProcess--;
						_pom = 5;
					}else if(_pom != 5){
						_zvysok--;
					}
				}
				ii = 0;
				while(ii<VELKOST_BUFFRA && _pocetNacitanychNull < _dlzka_vektoraF/2 && ii < _pocetDatPreProcess)	{
					_function_data[ii][0] = dajData(_premennaNaDruhu,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor);
					_pocitadlo++;
					//printf("_function_data[%d][0]=%c\n",ii,_function_data[ii][0]);		//DEEBUG VÝPIS
					ii++;
				}
				ii = 0;
				while(ii<VELKOST_BUFFRA && _pocetNacitanychJedn < _dlzka_vektoraF/2 && ii < _pocetDatPreProcess)	{
					_function_data[ii][1] = dajData(_premennaNaDruhu,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor);
					_pocitadlo++;
					//printf("_function_data[%d][1]=%c\n",ii,_function_data[ii][1]);		//DEEBUG VÝPIS
					ii++;
				}
				//printf("pocet poslnaných dát %d \n",ii);		//DEEBUG VÝPIS
				MPI_Ssend(&_function_data[0][0],ii*2, MPI_CHAR,(_process%(_pocetProc-2))+1, _process-(_pocetProc-3), MPI_COMM_WORLD);
				_process++;	
			}
			stopReceive(_pocetProc);
			printf("Root process rozparsoval %ld dát.\n", _pocitadlo);
			fclose(_subor);
			
		}
/*_____________END DEFINE ROOT PROCESS______________*/

	} else if (_pocetProc-1 == _rankP) { //LAST proc
		_zmenaFun_outputType[0] = prijmanieDat(_pocetProc,_zmenaFun_outputType);
		if(_zmenaFun_outputType[0] == -1)	{
			MPI_Finalize();
			return -1;
		}
	} else {
/*__________DEFINE ALL CALCULATE PROCESS_________*/
		int _velkostDat;
		MPI_Bcast(&_zmenaFun_outputType[0], 2, MPI_INT, 0,MPI_COMM_WORLD);		//do _zmenaFun_outputType[0] načítam voľbu pre deriváciu na akú hodnotu sa má zmeniť výstup funkcie
		if(_zmenaFun_outputType[0] == -1)	{
			/*#############################*/
			/* Ukončenie procesov s chybou */
			/*#############################*/
			MPI_Finalize();
			return -1;
		} else {
			/*##############################*/
			/* Korektné vykonávanie procesov*/
			/*##############################*/
			MPI_Status _status;
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
			MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
			_function_data = alloc_matrix(_velkostDat/2);
			do {
				MPI_Recv(&_function_data[0][0],_velkostDat, MPI_CHAR, 0,MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
				if(_status.MPI_TAG != END_TAG)	{
					and_and(_function_data,_velkostDat,_zmenaFun_outputType,_pocetProc);
				}
			}while(_status.MPI_TAG != END_TAG);
		}
	}
/*___________END DEFINE ALL CALCULATE PROCESS______*/
	
	
	
	
	
		
	if(_rankP == 0)	{ //ROOT proc 0
		free(_function_data);
		free(Data);
	} else if (_rankP != (_pocetProc-1)) { //Other proc
		MPI_Ssend(NULL,0, MPI_INT,_pocetProc-1, END_TAG, MPI_COMM_WORLD); /*Ukoncenie počúvania vlákna!*/
		free(Data);
		free(_function_data);
	}
	MPI_Finalize();
	return 0;
}
/*hahahahaha*/
