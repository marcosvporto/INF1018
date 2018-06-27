#include "geracod.h"
#include <stdlib.h>
#include <string.h>



int main(/* int argc, char **argv*/){


	FILE *myfp;
	int res;
	funcp sb; 
	if((myfp = fopen("programa", "r")) == NULL){
		perror("nao conseguiu abrir arquivo");
		exit(1);

	}
	sb = geracod(myfp);
	//res = (*sb)();
	//printf(" retorno da funcao sb%d\n",res);
	return 0;
}
