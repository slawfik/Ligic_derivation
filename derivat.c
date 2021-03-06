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
#define VELKOST_BUFFRA 256
#define OUT_NUMBER 1
#define OUT_VECTOR 0

char *Data;


void stopReceive(int pa_pocetProc)	{
	for(int i=1;i<pa_pocetProc-1;i++)	{
		MPI_Ssend(NULL,0, MPI_INT,i, END_TAG, MPI_COMM_WORLD);
	}
}

double dajPocetPremennych(FILE* pa_subor)	{
	unsigned long _pocetBytes;
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

int noParalel(int pa_pocetPrem, int pa_derivPodlaPrem, int* pa_zmenaFun_outputType,FILE* _subor)	{
	unsigned long _pocitadlo = 0;
	unsigned long _dlzka_vektoraF;
	unsigned long _pocetNacitanychJedn = 0;
	unsigned long _pocetNacitanychNull = 0;
	unsigned long _pom2;
	unsigned long _vysledok;
	unsigned long _pocetJednot;
	int a;
	int b;
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
		
	_dlzka_vektoraF = (unsigned long) pow(2,(double)pa_pocetPrem);
	printf("NO-PARALEL Počet premenných je: %d a dĺžka pravdivostného vektora je: %ld NO-PARALEL\n",pa_pocetPrem,_dlzka_vektoraF);
	printf("Derivácia f(1->0) / c%d(%d->%d) :\n",pa_derivPodlaPrem,!pa_zmenaFun_outputType[0],pa_zmenaFun_outputType[0]);
	_pom2 = (unsigned long) pow(2,(double)pa_derivPodlaPrem);
/*	if(!pa_zmenaFun_outputType[0])	{//== 0
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

	printf ( "Start Time %s\n", asctime (timeinfo) );
	time(&rawtime);
	timeinfo = localtime ( &rawtime );
	printf ( "End Time %s\n", asctime (timeinfo) );
	fclose(_subor);
	*/
	
	while(_pocitadlo != _dlzka_vektoraF)    {
        if(!pa_zmenaFun_outputType[0])  {//== 0

                        a = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor)-'0';
                        b = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor)-'0';

                        printf("%d",(!(a)*(b)));
                        _vysledok = !(a)*(b);
                        _pocitadlo = _pocitadlo+2;

        } else {
        //      while(_pocitadlo != _dlzka_vektoraF)    {
                        a = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor)-'0';
                        b = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor)-'0';

                        printf("%d",(!(a)*(b)));
                        _vysledok = !(a)*(b);
                        _pocitadlo = _pocitadlo+2;
        //      }
        }
        if(pa_zmenaFun_outputType[1] == OUT_VECTOR)     {
                //printf("DERIVACIA %ld\n",_vysledok);
        } else {
                if(_vysledok == 1)      {
                        _pocetJednot++;
                }
        }
	}
	printf("pocet jednotiek v derivácii %ld\n",_pocetJednot);

	printf ( "Start Time %s\n", asctime (timeinfo) );
	time(&rawtime);
	timeinfo = localtime ( &rawtime );
	printf ( "End Time %s\n", asctime (timeinfo) );
	fclose(_subor);

}

int prijmanieDat(int pa_pocetProc,int* pa_zmenaFun_outputType_derivPodlaPrem)	{
    int _velkostDat;
    int _pocitadloProc;
    int _process;
    int _end = 0;
    int _i;
    unsigned long _vysledok_NUM = 0;
    char* _vysledokDerivacie;
    MPI_Status _status;

    _pocitadloProc = pa_pocetProc-2;
    _process = (_pocitadloProc%(pa_pocetProc-2))+1;

    switch (pa_zmenaFun_outputType_derivPodlaPrem[1])	{

        case OUT_VECTOR:
            MPI_Probe(_process, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
            MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
            _vysledokDerivacie = (char*) malloc(sizeof(char)*_velkostDat);
            while(_end != (pa_pocetProc-2))	{
                MPI_Recv(&_vysledokDerivacie[0], _velkostDat, MPI_CHAR, _process, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
                MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
                _i = 0;
                if(_status.MPI_TAG != END_TAG)	{
                    while(_i != _velkostDat)	{
                        printf("TH Process: %d velkosť dát %d a výsledok _%d_!\n",_process,_velkostDat,_vysledokDerivacie[_i]);
                        _i++;
                    }
                }else {
                    _end++;	
                }
                _pocitadloProc++;
                if(_pocitadloProc == ((pa_pocetProc-2)*200))	{//aby nepretieklo počítanie procesov
                    _pocitadloProc = pa_pocetProc-2;
                }
                _process = (_pocitadloProc%(pa_pocetProc-2))+1;
            }
            free(_vysledokDerivacie);
            break;

        case OUT_NUMBER:
            while(_end != (pa_pocetProc-2))	{
                MPI_Recv(&_velkostDat, 1, MPI_INT, _process, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
                if(_status.MPI_TAG != END_TAG)	{
                    _vysledok_NUM+=_velkostDat;
                }else {
                    _end++;	
                }
                _pocitadloProc++;
                if(_pocitadloProc == ((pa_pocetProc-2)*200))	{//aby nepretieklo počítanie procesov
                    _pocitadloProc = pa_pocetProc-2;
                }
                _process = (_pocitadloProc%(pa_pocetProc-2))+1;
            }
            printf("\nPočet jednotiek vo výsledku derivácie _%ld_!\n",_vysledok_NUM);
            break;
    }	
}

int main(int argc, char** argv) {
/*---------DEFINE VARIABLES FOR EACH PROCESS------------*/
	int _rankP;
	int _pocetProc;
	int _zmenaFun_outputType_derivujPodla[3];
	double _pocetPrem;
	unsigned long _premennaNaDruhu;
/*---------END DEFINE VARIABLES FOR EACH PROCESS------------*/

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetProc);
	
/*_________________DEFINE ROOT PROCESS_________________*/
	if(_rankP == 0)	{
				
	  /*DEFINE VARIABLES FOR ROOT PROCESS*/
	  	unsigned long _dlzka_vektoraF;
		unsigned long _pocetNacitanychNull;
		unsigned long _pocetNacitanychJedn;
		unsigned long _pocetDatPreProcess;
		unsigned long _pocitadlo = 0;
		unsigned long _send_Data[3];
		FILE* _subor;
		int _process = (_pocetProc-2);
		int _zvysok;
		int _derivPodlaPrem;
		int _pom;
	  /*END DEFINE VARIABLES FOR ROOT PROCESS*/
	  
	  /*----INICIALIZÁCIA PREMENNÝCH----*/
		_derivPodlaPrem = atoi(argv[2]);			//Zistím premennú podla ktorej budeme derivovať
		_zmenaFun_outputType_derivujPodla[0] = atoi(argv[3]);	//na aku hodnotu sledujeme zmenu funkcie
		_subor = fopen(argv[1],"r");
		if(argv[4] != NULL && (0 == strcmp(argv[4],"-n")))	{						//ZYSTIM TYP VÝSTUPU
			_zmenaFun_outputType_derivujPodla[1] = OUT_NUMBER;
		} else {
			_zmenaFun_outputType_derivujPodla[1] = OUT_VECTOR;
		}
		_zmenaFun_outputType_derivujPodla[2] = _derivPodlaPrem;
		if(_subor == NULL)	{	
			printf("Nepodarilo sa otvoriť súbor %s\n",argv[1]);
			_zmenaFun_outputType_derivujPodla[0] = -1;
			MPI_Bcast(&_zmenaFun_outputType_derivujPodla[0], 3, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		}
		_pocetPrem = dajPocetPremennych(_subor);
		_dlzka_vektoraF = (unsigned long) pow(2,_pocetPrem);
	  /*----END INIT PREMENNÝCH----------*/
		
		if(_pocetProc == 1)	{  /*IF NO paralel*/
			noParalel(_pocetPrem,_derivPodlaPrem,_zmenaFun_outputType_derivujPodla,_subor);
                        fclose(_subor);
			MPI_Finalize();
			return 0;
		
		} else if((_pocetProc-2) > _dlzka_vektoraF/2 | _pocetProc == 2) { /* ERROR INPUT BAD PROCESS NUMBER */
			_zmenaFun_outputType_derivujPodla[0] = -1;
			printf("Error výpočet môže byť rozdelený maximálne do %d procesov a nesmú sa použiť 2 procesy!\n",(_dlzka_vektoraF/2)+1);
			MPI_Bcast(&_zmenaFun_outputType_derivujPodla[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		
		} else if((double)_derivPodlaPrem >= _pocetPrem ) { /* ERROR INPUT BAD VARIABLE FOR DERIVATION */
			_zmenaFun_outputType_derivujPodla[0] = -1;
			printf("Error premená musí byť menšia ako počet všetkých premenných!! Počet premenných je %.1f\n",_pocetPrem);
			MPI_Bcast(&_zmenaFun_outputType_derivujPodla[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			MPI_Finalize();
			return -1;
		
		}else {	/*Paralelné spracovanie*/
			//MPI_Buffer_attach(b,(VELKOST_BUFFRA*2)+MPI_BSEND_OVERHEAD);
			printf("Počet premenných je: %.1f a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
			printf("Derivácia f(1->0) / c%d(%d->%d) :\n",_derivPodlaPrem,!_zmenaFun_outputType_derivujPodla[0],_zmenaFun_outputType_derivujPodla[0]);
			MPI_Bcast(&_zmenaFun_outputType_derivujPodla[0], 2, MPI_INT, _rankP,MPI_COMM_WORLD);
			
			_pocetDatPreProcess = (_dlzka_vektoraF/2)/(_pocetProc-2);
			_zvysok = (_dlzka_vektoraF/2)%(_pocetProc-2);
			_pocetNacitanychJedn = 0;
			_pocetNacitanychNull = 0;
			if(_pocetDatPreProcess < VELKOST_BUFFRA)	{
				_pocetDatPreProcess++;
				_pom = -1;
			}
		
			while(_pocitadlo != _dlzka_vektoraF)	{		/* DATA PARSING */
				if(_process == ((_pocetProc-2)*200))	{
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
				
				if(_pocetDatPreProcess < VELKOST_BUFFRA)	{
					_send_Data[0] = _pocetNacitanychNull;
					_send_Data[1] = _pocetNacitanychJedn;
					_send_Data[2] = _pocetDatPreProcess;
					_pocetNacitanychNull += _pocetDatPreProcess;
					_pocetNacitanychJedn += _pocetDatPreProcess;
					_pocitadlo += (_pocetDatPreProcess*2);
				} else {
					_send_Data[0] = _pocetNacitanychNull;
					_send_Data[1] = _pocetNacitanychJedn;
					_send_Data[2] = VELKOST_BUFFRA;
					_pocetNacitanychNull += VELKOST_BUFFRA;
					_pocetNacitanychJedn += VELKOST_BUFFRA;
					_pocitadlo += (VELKOST_BUFFRA*2);
				}
				//printf("pocitadlo pocitadlo %ld \n",_pocitadlo);
				MPI_Ssend(&_send_Data[0],3, MPI_UNSIGNED_LONG,(_process%(_pocetProc-2))+1, (_process%(_pocetProc-2))+1, MPI_COMM_WORLD);
				_process++;
			}
			stopReceive(_pocetProc);
			printf("Root process rozparsoval %ld dát.\n", _pocitadlo);
			fclose(_subor);
			
		}
/*_____________END DEFINE ROOT PROCESS______________*/

	} else if (_pocetProc-1 == _rankP) { //LAST proc
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );

            MPI_Bcast(_zmenaFun_outputType_derivujPodla, 3, MPI_INT, 0,MPI_COMM_WORLD);
            if(_zmenaFun_outputType_derivujPodla[0] == -1)	{
                MPI_Finalize();
                return -1;
            } else {
                prijmanieDat(_pocetProc,_zmenaFun_outputType_derivujPodla);

                printf( "Start Time %s\n", asctime(timeinfo) );
                time( &rawtime );
                timeinfo = localtime( &rawtime );
                printf( "End Time %s\n", asctime(timeinfo) );
            }
	} else {
/*__________DEFINE ALL CALCULATE PROCESS_________*/
            unsigned long _receive_data[3];
            unsigned long _vysledokNum = 0;
            char* _vysledok;
            char a;
            char b;
            FILE* _subor;
            MPI_Status _status;

            MPI_Bcast(&_zmenaFun_outputType_derivujPodla[0], 3, MPI_INT, 0,MPI_COMM_WORLD);	//do _zmenaFun_outputType_derivujPodla[0] načítam voľbu pre deriváciu na akú hodnotu sa má zmeniť výstup funkcie
            if(_zmenaFun_outputType_derivujPodla[0] == -1)	{
                /*#############################*/
                /* Ukončenie procesov s chybou */
                /*#############################*/
                MPI_Finalize();
                return -1;
            } else {
                /*##############################*/
                /* Korektné vykonávanie procesov*/
                /*##############################*/
                _subor = fopen(argv[1],"r");
                _vysledok = (char*) malloc(sizeof(char)*VELKOST_BUFFRA);
                _premennaNaDruhu = pow(2,(double)_zmenaFun_outputType_derivujPodla[2]);
                do {
                    MPI_Recv(&_receive_data[0],3, MPI_UNSIGNED_LONG, 0,MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
                    if(_status.MPI_TAG != END_TAG)	{
                        //printf("%d tag je ----> puzite nuly %ld,pouzite jednotky %ld, pocet %ld\n",_status.MPI_TAG,_receive_data[0],_receive_data[1],_receive_data[2]);
                        switch(_zmenaFun_outputType_derivujPodla[1])	{

                            case OUT_VECTOR:
                                for(int i =0;i<_receive_data[2];i++)	{
                                    a = dajData(_premennaNaDruhu,&_receive_data[0],&_receive_data[1],0,_subor);
                                    b = dajData(_premennaNaDruhu,&_receive_data[0],&_receive_data[1],1,_subor);
                                    a = a - '0';
                                    b = b - '0';
                                    if(_zmenaFun_outputType_derivujPodla[0] == 0)	{
                                            _vysledok[i] = (!a*b);
                                    } else {
                                            _vysledok[i] = (a*!b);
                                        }
                                }
                                MPI_Ssend(&_vysledok[0],(int)_receive_data[2], MPI_CHAR,_pocetProc-1,_status.MPI_TAG, MPI_COMM_WORLD);
                                break;

                            case OUT_NUMBER:
                                for(int i =0;i<_receive_data[2];i++)	{
                                    a = dajData(_premennaNaDruhu,&_receive_data[0],&_receive_data[1],0,_subor);
                                    b = dajData(_premennaNaDruhu,&_receive_data[0],&_receive_data[1],1,_subor);
                                    a = a - '0';
                                    b = b - '0';
                                    if(!_zmenaFun_outputType_derivujPodla[0] && (!a*b))	{//== 0
                                            _vysledokNum++;
                                    } else if (_zmenaFun_outputType_derivujPodla[0] && (a*!b)){//== 1
                                            _vysledokNum++;
                                    }
                                }
                                MPI_Ssend(&_vysledokNum,1, MPI_INT,_pocetProc-1,_status.MPI_TAG, MPI_COMM_WORLD);
                                break;
                        }
                    }
                }while(_status.MPI_TAG != END_TAG);

                MPI_Ssend(NULL,0, MPI_INT,_pocetProc-1, END_TAG, MPI_COMM_WORLD); /*Ukoncenie počúvania vlákna!*/
                free(_vysledok);
                fclose(_subor);
            }
	}
/*___________END DEFINE ALL CALCULATE PROCESS______*/

    MPI_Finalize();
    return 0;
}
