// tomaspusuje!!
//som ide commit?
//dalsi kommit je tu!
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>

#define SUBOR "input.txt"
#define VELKOST_BUFFRA 512

char *Data;

int isNullOrOne(int pa_index,int pa_premenna)	{
//	decimal to bin
	int _pom[50];
	int _i = 0;
	if(pa_index == 0)	{
		return 0;
	} else {
		while(pa_index != 0)	{
			_pom[_i] = pa_index % 2;
			pa_index = pa_index/2;
			_i++;
		}
		_i--;
		if(_i<pa_premenna)	{
			return 0;
		} else {
			return _pom[pa_premenna];
		}
	}
}

int and_and(char **pa_functionData,int pa_pocetDat,int pa_zmenaPremennej)	{
	char b;
	int _rankP,aa,bb;
	char a;
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	for(int _i = 0;_i<(pa_pocetDat/2);_i++)	{
		a = pa_functionData[_i][pa_zmenaPremennej];
		b = pa_functionData[_i][!pa_zmenaPremennej];
		aa = atoi(&a);
		fflush(stdout);
		bb = atoi(&b);
		//printf("zmenaPremennejNa %d_!%d ; znaky a%c_b%c ; int aa%d_bb%d\n",pa_zmenaPremennej,!pa_zmenaPremennej,a,b,aa,bb);
		printf("PROCESS: %d  %d_*_!%d = %d\n",_rankP,bb,aa,(bb*!aa));
	}
}

void stopReceive()	{
	for(int i=1;i<5;i++)	{
		MPI_Ssend(NULL,0, MPI_INT,i, 0, MPI_COMM_WORLD);
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

int main(int argc, char** argv) {
	int _rankP, _pocetPrem,_pocetProc,_pom1;
	unsigned long _dlzka_vektoraF;
	unsigned long _pocetNULLDatPreProcess, _pocetDatPreProcess, _pom2, _pocitadlo;

	char** _function_data;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetProc);
	
	/*DEFINE ROOT PROCESS*/
	if(_rankP == 0)	{		
	  /*DEFINE VARIABLES FOR ROOT PROCESS*/
		unsigned long _pocetNacitanychNull;
		unsigned long _pocetNacitanychJedn;
		int _process = 4,ii, _derivPodlaPrem,_poziciaVSubore;
		FILE* _subor;
	  /*END DEFINE VARIABLES FOR ROOT PROCESS*/
	  /*INICIALIZÁCIA PREMENNÝCH*/
		_pocetPrem = atoi(argv[1]);	//Zistím počet premenných danej funkcie
		if(_pocetPrem <= 2)	{
			printf("Pre paralelný výpočet musí byť počet premenných väčší ako 2 !!");
			stopReceive();
			MPI_Finalize();
			return -1;
		}
		_subor = fopen(SUBOR,"r");
		_dlzka_vektoraF = (unsigned long) pow(2,(double)_pocetPrem);
		_function_data = alloc_matrix(VELKOST_BUFFRA);

		printf("Počet premenných je: %d a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
		printf("Zadajte zadajte číslo zodpovedajúcej premennej podľa ktorej chcete derivovať:(0 alebo 1 alebo 20... n)\n");
		scanf("%d",&_derivPodlaPrem);
		while(_derivPodlaPrem > (_pocetPrem-1))	{
			scanf("%d",&_derivPodlaPrem);
		}
		printf("f(1->0)/c%d(x -> x)   Zadajte na aku hodnotu sa má zmenit premenná c%d:\n",_derivPodlaPrem,_derivPodlaPrem);
		do	{
			scanf("%d",&_pom1);
		}while(_pom1 > 1);
		MPI_Bcast(&_pom1, 1, MPI_INT, _rankP,MPI_COMM_WORLD);
		
		_pocitadlo = 0;
		_pocetDatPreProcess = _dlzka_vektoraF/4;
		_pom2 = pow(2,(double)_derivPodlaPrem); //premenna podla ktorej chceme 2^derivovať
		_pocetNacitanychJedn = 0;
		_pocetNacitanychNull = 0;
	  /*END INICIALIZÁCIA PREMENNÝCH*/
		while(_pocitadlo != _dlzka_vektoraF)	{
			ii = 0;
			while(ii<(_pocetDatPreProcess/2) && ii<VELKOST_BUFFRA)	{
				_function_data[ii][0] = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor);
				_pocitadlo++;
				//printf("_function_data[0][%d]=%d\n",ii,_function_data[0][ii]);
				ii++;
			}
			ii = 0;
			while(ii<(_pocetDatPreProcess/2) && ii<VELKOST_BUFFRA)	{
				_function_data[ii][1] = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor);
				_pocitadlo++;
				//printf("_function_data[1][%d]=%d\n",ii,_function_data[1][ii]);
				ii++;
			}
			if(ii == (_pocetDatPreProcess/2))	{
				MPI_Ssend(&_function_data[0][0],(int)_pocetDatPreProcess, MPI_CHAR,(_process%4)+1, _process-3, MPI_COMM_WORLD);
			} else {
				MPI_Ssend(&_function_data[0][0],VELKOST_BUFFRA*2, MPI_CHAR,(_process%4)+1, _process-3, MPI_COMM_WORLD);
			}
			_process++;
		}
		stopReceive();
		printf("Pocet rozparsovaných dát je %ld\n", _pocitadlo);
		fclose(_subor);
	/*END DEFINE ROOT PROCESS*/
	} else {
		int _velkostDat;
		MPI_Bcast(&_pom1, 1, MPI_INT, 0,MPI_COMM_WORLD);
		MPI_Status _status;
		do {
			MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &_status);
			MPI_Get_count(&_status, MPI_CHAR, &_velkostDat);
			if(_status.MPI_TAG <= 4 && _status.MPI_TAG != 0)	{
				printf("Velkost príjmaných dát ktoré zistil prijmateľ _%d_ a MPI_TAG _%d_ a premenná sa ma zmeniť na _%d_\n",_velkostDat,_status.MPI_TAG,_pom1);
				_function_data = alloc_matrix(_velkostDat/2);
			}
			MPI_Recv(&_function_data[0][0],_velkostDat, MPI_CHAR, 0,_status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(_status.MPI_TAG != 0)	{
				and_and(_function_data,_velkostDat,_pom1);
				/*printf("_function_data[0][0]=%d\n",_function_data[0][0]);
				printf("_function_data[1][0]=%d\n",_function_data[0][1]);*/
			}
		}while(_status.MPI_TAG != 0);
	}

	if(_rankP == 0)	{
		free(_function_data); //dealokuj ináč
		free(Data);
	}
	MPI_Finalize();
}

	/*NO PARALEL*/
				/*		int _pocit1 = 0, _pocit2 = 0;
						for(int _i = 0;_i < _dlzka_vektoraF;_i++)	{
							if(isNullOrOne(_i,_derivPodlaPrem) == _pom1)	{
								_hodnota_funkcie_prem1[_pocit1]	= _vektor_funk[_i];
								_pocit1++;
							}	else {
								_hodnota_funkcie_prem0[_pocit2]	= _vektor_funk[_i];
								_pocit2++;
							}
						}
						and_and(_hodnota_funkcie_prem0,_hodnota_funkcie_prem1,_dlzka_vektoraF/2);
				*/
