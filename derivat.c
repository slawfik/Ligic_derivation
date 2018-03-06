#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define SUBOR "input.txt"
#define VELKOST_BUFFRA 256

/*
 * pocetPremennych -- process max -> min -- 	vektorLen
 * 		2				2 - 1						4
 * 		3				4 - 2 - 1					8
 * 		4				8 - 4 - 2 - 1				16
 * 		5				16 - 8 - 4 - 2 - 1			32
 * */


int _pow(unsigned long long* pa_dlzka_vektoraF,int pa_exponent,int pa_rank){
	int _i = 0, _pom = 1;
	//MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
	if(pa_rank == 0)	{
		while(_i != pa_exponent){	/*pa_exponent == pocet premenných*/
			*pa_dlzka_vektoraF = ((*pa_dlzka_vektoraF) * 2);
			_i++;
		}
	} else {
		while(_i != pa_exponent){
			_pom = (_pom * 2);
			_i++;
		}
		return _pom;
	}
}

int nacitajFunkciu(unsigned long long* pa_dlzka_vektoraF,int* pa_vektorF)	{
	FILE* _subor = fopen(SUBOR,"r");
	char _buffer[4];
	int _i = 0;
	if(_subor == NULL)	{
		perror("Subor input.txt sa nepodarilo otvoriť!");
		return -1;
	}	else {
		while(_i != *pa_dlzka_vektoraF && fgets(_buffer,4,_subor)!=NULL)	{
			pa_vektorF[_i] = atoi(_buffer);
			//printf("%d-->%d\n",_i,pa_vektorF[_i]);
			_i++;
		}
		fclose(_subor);
		return 0;
	}
}

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

int and_and(int* pa_vektor1,int* pa_vektor2,int pa_maxLen)	{
	printf("Výsledok:\n");
	for(int _i = 0;_i<pa_maxLen;_i++)	{
		if(pa_vektor2[_i] == 0)	{
			printf(" %d \n",(pa_vektor1[_i]*1));
		}	else {
			printf(" %d \n",(pa_vektor1[_i]*0));
		}
	}
}

int **alloc_matrix(int riadky) {	/*vytvorý maticu (n*2) */
    int *data = (int *)malloc(riadky*2*sizeof(int));
    int **_function_data = (int **)malloc(riadky*sizeof(int*));
    for (int i=0; i<riadky; i++)	{
        _function_data[i] = &(data[2*i]);
	}
    return _function_data;
}

int dajData(unsigned long pa_premNa2,unsigned long* pa_nacitaneNULL,unsigned long* pa_nacitaneJEDN,int pa_type,FILE* pa_subor)	{
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
	int _rankP, _pocetPrem,_pocetProc,_derivPodlaPrem,_pom1,_poziciaVSubore;
	unsigned long _dlzka_vektoraF;
	unsigned long _pocetNULLDatPreProcess, _pocetDatPreProcess, _pom2, _pocitadlo;

	int** _function_data;

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetProc);
	_function_data = alloc_matrix(VELKOST_BUFFRA);
	
	/*DEFINE ROOT PROCESS*/
	if(_rankP == 0)	{		
		/*DEFINE VARIABLES FOR ROOT PROCESS*/
		unsigned long _pocetNacitanychNull;
		unsigned long _pocetNacitanychJedn;
		int _process = 4,ii;
		FILE* _subor;
		/*END DEFINE VARIABLES FOR ROOT PROCESS*/

		_subor = fopen(SUBOR,"r");
		_pocetPrem = atoi(argv[1]);	//Zistím počet premenných danej funkcie
		_dlzka_vektoraF = (unsigned long) pow(2,(double)_pocetPrem);

		printf("Počet premenných je: %d a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
		printf("Zadajte zadajte číslo zodpovedajúcej premennej podľa ktorej chcete derivovať:(0 alebo 1 alebo 20... n)\n");
		scanf("%d",&_derivPodlaPrem);
		while(_derivPodlaPrem > (_pocetPrem-1))	{
			scanf("%d",&_derivPodlaPrem);
		}
		printf("f(1->0)/c%d(x -> x)   Zadajte na aku hodnotu sa má zmenit premenná c%d:\n",_derivPodlaPrem,_derivPodlaPrem);
		do	{
			scanf("%d",&_pom1);
		}while(_pom1 > 1 );
		
		_pocitadlo = 0;
		_pocetDatPreProcess = _dlzka_vektoraF/4;
		_pom2 = pow(2,(double)_derivPodlaPrem);
		_pocetNacitanychJedn = 0;
		_pocetNacitanychNull = 0;
		
		while(_pocitadlo != _dlzka_vektoraF)	{
			ii = 0;
			while(ii<(_pocetDatPreProcess/2) && ii<VELKOST_BUFFRA)	{
				_function_data[0][ii] = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,0,_subor);
				_pocitadlo++;
				ii++;
			}
			ii = 0;
			while(ii<(_pocetDatPreProcess/2) && ii<VELKOST_BUFFRA)	{
				_function_data[1][ii] = dajData(_pom2,&_pocetNacitanychNull,&_pocetNacitanychJedn,1,_subor);
				_pocitadlo++;
				//printf("---%ld  aaaaalo ii %d\n",_pocitadlo,ii);
				//printf("_function_data[1][%d]=%d\n",ii,_function_data[1][ii]);
				ii++;
			}
			MPI_Ssend(&_function_data[0][0],VELKOST_BUFFRA*2, MPI_INT,(_process%4)+1, 0, MPI_COMM_WORLD);
			_process++;
		}
		//printf("Pocet rozparsovaných dát je %ld\n", _pocitadlo);
		fclose(_subor);
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
	/*END DEFINE ROOT PROCESS*/
	} else {/*
		int velkost = 0;
		MPI_Status status;
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &velkost);
		printf("Velkost príjmaných dát ktoré zistil prijmateľ %d\n",velkost);
		//_function_data = alloc_matrix(velkost/2);*/
		
		MPI_Recv(&_function_data[0][0],VELKOST_BUFFRA*2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int i=0;i<2;i++)	{
			printf("proc %d _function_data[0][%d]=%d\n",_rankP,i,_function_data[0][i]);
			printf("proc %d _function_data[1][%d]=%d\n",_rankP,i,_function_data[1][i]);
		}
	}


	/*if(_rankP == 0)	{
		//free(_function_data); dealokuj ináč
	}*/
	MPI_Finalize();
}

/* if (_rankP == 0) {
	// If wex are rank 0, set the number to -1 and send it to process 1
	data[1][0] = -55551111;
	MPI_Send(&data[0][0],(3*2), MPI_INT, 1, 0, MPI_COMM_WORLD);
  } else if (_rankP == 1) {
	printf("_____________________Process 1 received number %d from process 0\n", data[1][0]);
	MPI_Recv(&data[0][0],(3*2), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	printf("_____________________Process 1 received number %d from process 0\n", data[1][0]);
	
	/*for(int c = 0;c < 3;c++)		{
		for(int i = 0;i < 2;i++)		{
			printf("[%d][%d]__received number %d from process 0\n",c,i,data[c][i]);
		}
	}
}*/
