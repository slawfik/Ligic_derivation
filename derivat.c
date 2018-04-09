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
#define SUBOR "input.txt"
#define VELKOST_BUFFRA 10

char *Data;
//char* zalohaDAta;

int and_and(char **pa_functionData,int pa_pocetDat,int pa_zmenaPremennej)	{
	char b;
	char a;
	int _rankP,aa,bb;
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	for(int _i = 0;_i<(pa_pocetDat/2);_i++)	{
		a = pa_functionData[_i][pa_zmenaPremennej];
		b = pa_functionData[_i][!pa_zmenaPremennej];
		aa = a-'0';
		bb = b-'0';
		//printf("PROCESS: %d  %d_*_!%d = %d\n",_rankP,bb,aa,(bb*!aa));
		pa_functionData[_i][0] = (bb*!aa);
		printf("_toto je int int %d\n",pa_functionData[_i][0]);
	}
	//MPI_Ssend(&pa_functionData[0][0],pa_pocetDat, MPI_CHAR,0,NO_END_TAG, MPI_COMM_WORLD);
}

void stopReceive(int pa_pocetProc)	{
	for(int i=1;i<pa_pocetProc;i++)	{
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
	unsigned long _polohaVsubore = 0,_pom;
	if(pa_type == 0)	{
		_pom = *pa_nacitaneNULL/pa_premNa2;
		_polohaVsubore = *pa_nacitaneNULL+(_pom*pa_premNa2);
		*pa_nacitaneNULL=*pa_nacitaneNULL+1;
		//printf("---- 0----- _polohavSUBIRE %ld\n",_polohaVsubore);
	}	else {
		_pom = *pa_nacitaneJEDN/pa_premNa2;
		_polohaVsubore =*pa_nacitaneJEDN+((_pom+1)*pa_premNa2);
		*pa_nacitaneJEDN=*pa_nacitaneJEDN+1;
		//printf("---- 1----- _polohavSUBIRE %ld\n",_polohaVsubore);
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

void* prijmanieDat()	{
	MPI_Status _status;
	short _end = 0;
	char** _vysledokDerivacie;
	int _procesy = 4;
	int _velkostDat;

	MPI_Probe((_procesy%4)+1, MPI_ANY_TAG, MPI_COMM_WORLD, &_status); /*zme tuna civlo procesu od ktorího príjma data!!*/
	MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
	_vysledokDerivacie = alloc_matrix(_velkostDat);

	MPI_Recv(&_vysledokDerivacie[0][0],_velkostDat, MPI_CHAR, (_procesy%4)+1,_status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	while(1) {
			_procesy++;
			if(_procesy==8){ break;}
			
			for(int i = 0;i<(_velkostDat/2);i++)	{
				printf("%d cakam,,,,Process %d: = %d\n",(_procesy%4)+1,((_procesy-1)%4)+1,_vysledokDerivacie[i][0]);
			}	
		
		MPI_Recv(&_vysledokDerivacie[0][0],_velkostDat, MPI_CHAR, (_procesy%4)+1,_status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	//free(_vysledokDerivacie);
	//free(Data);
}

int main(int argc, char** argv) {
	int _rankP;
	int _pocetProc;
	int _pom1;
	unsigned long _pocetNULLDatPreProcess;
	unsigned long _pocetDatPreProcess;
	unsigned long _premennaNaDruhu;
	unsigned long _pocitadlo;
	char** _function_data;
	double _pocetPrem;


	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetProc);
	
/*##########DEFINE ROOT PROCESS##########*/
	if(_rankP == 0)	{
				
	  /*DEFINE VARIABLES FOR ROOT PROCESS*/
		pthread_t _vlaknoPrePrijmanie;
	  	unsigned long _dlzka_vektoraF;
		unsigned long _pocetNacitanychNull;
		unsigned long _pocetNacitanychJedn;
		unsigned long _pocetPrijati;
		FILE* _subor;
		unsigned long _process = (_pocetProc-1);
		int ii;
		int _derivPodlaPrem;
		int _poziciaVSubore;
	  /*END DEFINE VARIABLES FOR ROOT PROCESS*/
	  
/*----INICIALIZÁCIA PREMENNÝCH----*/
		_derivPodlaPrem = atoi(argv[2]);								//Zistím premennú podla ktorej budeme derivovať
		_pom1 = atoi(argv[3]);											//na aku hodnotu sledujeme zmenu funkcie
		_subor = fopen(argv[1],"r");
			if(_subor == NULL)	{	
				printf("Nepodarilo sa otvoriť súbor %s\n",argv[1]);
				MPI_Finalize();
				return -1;
			}
		_pocetPrem = dajPocetPremennych(_subor);
		
		if(_pocetProc == 1)	{/*IF NO paralel*/
			noParalel(_pocetPrem,_derivPodlaPrem,_pom1,_subor);
			MPI_Finalize();
			return 0;
		} else {			/*ELSE paralel*/

			_dlzka_vektoraF = (unsigned long) pow(2,_pocetPrem);
			_function_data = alloc_matrix(VELKOST_BUFFRA);

			printf("Počet premenných je: %.1f a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
			printf("Derivácia f(1->0) / c%d(%d->%d) :\n",_derivPodlaPrem,!_pom1,_pom1);
			MPI_Bcast(&_pom1, 1, MPI_INT, _rankP,MPI_COMM_WORLD);
			
			_pocitadlo = 0;
			_pocetDatPreProcess = (_dlzka_vektoraF/2)/(_pocetProc-1);
			int zvysok = (_dlzka_vektoraF/2)%(_pocetProc-1);
			_premennaNaDruhu = pow(2,(double)_derivPodlaPrem); //premenna podla ktorej chceme 2^derivovať
			_pocetNacitanychJedn = 0;
			_pocetNacitanychNull = 0;
/*----END INICIALIZÁCIA PREMENNÝCH----*/

	//	    pthread_create(&_vlaknoPrePrijmanie,NULL,prijmanieDat,NULL);
			if(_pocetDatPreProcess < VELKOST_BUFFRA)	{
				_pocetDatPreProcess++;
			}
			while(_pocitadlo != _dlzka_vektoraF)	{
				if((_pocetDatPreProcess-1) < VELKOST_BUFFRA)	{
					if(zvysok == 0 && _pom1 != 5)	{
						_pocetDatPreProcess--;
						_pom1 = 5;
					}else if(_pom1 != 5){
						zvysok--;
					}
				}
				ii = 0;
				while(ii<VELKOST_BUFFRA && _pocetNacitanychNull <= _dlzka_vektoraF/2 && ii < _pocetDatPreProcess)	{
					_function_data[ii][0] = dajData(_premennaNaDruhu,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor);
					_pocitadlo++;
					//printf("_function_data[%d][0]=%c\n",ii,_function_data[ii][0]);
					ii++;
				}
				ii = 0;
				while(ii<VELKOST_BUFFRA && _pocetNacitanychJedn <= _dlzka_vektoraF/2 && ii < _pocetDatPreProcess)	{
					_function_data[ii][1] = dajData(_premennaNaDruhu,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor);
					_pocitadlo++;
					//printf("_function_data[%d][1]=%c\n",ii,_function_data[ii][1]);
					ii++;
				}
				MPI_Ssend(&_function_data[0][0],ii*2, MPI_CHAR,(_process%(_pocetProc-1))+1, _process-(_pocetProc-2), MPI_COMM_WORLD);

				_process++;
			}
			stopReceive(_pocetProc);
			printf("Pocet rozparsovaných dát je %ld\n", _pocitadlo);
			fclose(_subor);
	//		pthread_join(_vlaknoPrePrijmanie,NULL);
/*##########END DEFINE ROOT PROCESS##########*/
	}
	} else {		
		int _velkostDat;
		MPI_Bcast(&_pom1, 1, MPI_INT, 0,MPI_COMM_WORLD);
		MPI_Status _status;
		do {
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
			MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
			if(_status.MPI_TAG <= (_pocetProc-1) && _status.MPI_TAG != 0)	{
				printf("Velkost príjmaných dát ktoré zistil prijmateľ _%d_ a MPI_TAG _%d_ a premenná sa ma zmeniť na _%d_\n",_velkostDat,_status.MPI_TAG,_pom1);
				_function_data = alloc_matrix(_velkostDat/2);
			}
			MPI_Recv(&_function_data[0][0],_velkostDat, MPI_CHAR, 0,_status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(_status.MPI_TAG != 0)	{
				and_and(_function_data,_velkostDat,_pom1);
			}
		}while(_status.MPI_TAG != 0);
		printf("koncim %d\n",_rankP);
	}
	
if(_rankP == 0)	{ //proc 0
		printf("posledne vzdy!!!\n");
		free(_function_data);
		free(Data);
	} else { //proc 1 2 3 4
		//printf("Ostatne procesy sa ukončilil\n");
		//MPI_Send(NULL,0, MPI_INT,0, END_TAG, MPI_COMM_WORLD); /*Ukoncenie počúvania vlákna!*/
		free(Data);
		free(_function_data);
	}
	MPI_Finalize();
}


/*
 * 
 * 	if(_rankP == 1)	{ //proc 1
//		MPI_Ssend(NULL,0, MPI_INT,0, END_TAG, MPI_COMM_WORLD); /*Ukoncenie počúvania vlákna!
		printf("Ukonči prijmanie dát!\n");
		free(Data);
		free(_function_data);
	} else 
	*/
