#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#define SUBOR "input.txt"

int _pow(unsigned long long* pa_dlzka_vektoraF,int pa_exponent){
	int _i = 0, _rank, _pom = 1;
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
	if(_rank == 0)	{
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

int main(int argc, char** argv) {
	int _rankP, _pocetPrem, _pom,_pom1;
	unsigned long long _dlzka_vektoraF = 1;
	int* _vektor_funk;
	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &_rankP);
	MPI_Comm_size(MPI_COMM_WORLD, &_pocetPrem);
	
	if(_rankP == 0)	{
		_pocetPrem = atoi(argv[1]);	/*Zistím počet premenných danej funkcie*/
		_pow(&_dlzka_vektoraF,_pocetPrem);
		
		int _hodnota_funkcie_prem1[_dlzka_vektoraF/2] , _hodnota_funkcie_prem0[_dlzka_vektoraF/2];
		_vektor_funk = (int*) malloc(sizeof(int)*_dlzka_vektoraF);
		printf("Počet premenných je: %d a dĺžka pravdivostného vektora je: %ld\n",_pocetPrem,_dlzka_vektoraF);
		nacitajFunkciu(&_dlzka_vektoraF,_vektor_funk);	/*NACITANIE VUNKCIE DO POLA*/
		printf("Zadajte zadajte číslo zodpovedajúcej premennej podľa ktorej chcete derivovať:\n");
		for(int i = 0;i<_pocetPrem;i++)	{
			printf("%d.......c%d\n",i,i);
		}
		scanf("%d",&_pom);
		while(_pom > _pocetPrem)	{
			scanf("%d",&_pom);
		}
		printf("f(1->0)/c%d(x -> x)   Zadajte na aku hodnotu sa má zmenit premenná c%d:\n",_pom,_pom);
		scanf("%d",&_pom1);
		/*no paralel*/
		int _pocit1 = 0, _pocit2 = 0;
		for(int _i = 0;_i < _dlzka_vektoraF;_i++)	{
			if(isNullOrOne(_i,_pom) == _pom1)	{
				_hodnota_funkcie_prem1[_pocit1]	= _vektor_funk[_i];
				_pocit1++;
			}	else {
				_hodnota_funkcie_prem0[_pocit2]	= _vektor_funk[_i];
				_pocit2++;
			}
		}
		and_and(_hodnota_funkcie_prem0,_hodnota_funkcie_prem1,_dlzka_vektoraF/2);
//*/
	}
	
	//int* _receive_Fdata = (int*)malloc(sizeof(int)*(_dlzka_vektoraF/4));

	if(_rankP == 0)	{
		free(_vektor_funk);
	}
	MPI_Finalize();
}


