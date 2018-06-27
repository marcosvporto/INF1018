#include "geracod.h"
#include <stdlib.h>
#include <string.h>


#define MAX_BYTES 1
#define MAX_TAM_LINHA 200
#define MAX_LINHAS 20



void dump (void *p, int n) {
  unsigned char *p1 = p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}

void liberacod(void *pf){

	free(pf);

} 


static void error (const char *msg, int line){

	fprintf(stderr, "erro %s na linha %d\n",msg,line);
	exit(EXIT_FAILURE);
}


void preenche_cte(int cte , unsigned char *v){
		int i;	
		for(i=0;i<4;i++){
			v[i]= (unsigned char *) (cte>>(i*8));
		}


} 

funcp geracod(FILE *myfp){

	unsigned char *codigo ={"0x55","0x48","0x89","0xe5","0x48","0x83","0xec","0x10","0xb8","0x00","0x00","0x00","0x00","0xc9","0xc3"};//(unsigned char *)malloc(MAX_BYTES);
	/*unsigned char prologo[8] = { "0x55","0x48","0x89","0xe5","0x48","0x83","0xec","0x10" };
	unsigned char epilogo[2] = { "0xc9","0xc3" };
	unsigned char retv[2]    = {"0x8b","0x45"};
	unsigned char retv1[1]   = {"0xfc"};
	unsigned char retv2[1]   = {"0xf8"};
	unsigned char retv3[1]   = {"0xf4"};
	unsigned char retv4[1]   = {"0xf0"};
	unsigned char retp[1]    = {"0x89"};
	unsigned char retp1[]   = {"0xf8"};
	unsigned char retp2[]   = {"0xf0"};
	unsigned char retcte[]  = {"0xb8"};	
	char *linha = (char*)malloc(MAX_TAM_LINHA);
	int byte_corrente = 0;
	int line = 1;
	int c;
	
	/*if(codigo=NULL){
		fprintf(stderr, "Falta de memória ao alocar bloco de codigo\n");
		exit(0);
	}	
	memcpy(codigo,prologo,sizeof(prologo));
	byte_corrente += sizeof(prologo);

	
	/*while ((c = fgetc(myfp)) != EOF ){
		
		switch(c){

			
			case 'r': {/* retorno /
				char var0;
				int idx0;
				int i;
				if (fscanf (myfp, "et %c%d", &var0, &idx0) != 2)
					error("comando invalido",line);
				if(var0=='$'){
					codigo[byte_corrente] = retcte[0];
					byte_corrente++;
					unsigned char v[4];
					preenche_cte (idx0,v);
					memcpy(codigo+byte_corrente,v,sizeof(v));
					byte_corrente += sizeof(v);
					memcpy(codigo+byte_corrente,epilogo,sizeof(epilogo));
					byte_corrente += sizeof(epilogo);
					
				}	
				printf("%d ret %c%d\n", line,var0,idx0);
				break;
			}
			case 'v':
			case 'p':{/* atribuicao e op. aritmetica /
				char var0 = c, var1, op;
				int idx0, idx1;
				if(fscanf(myfp, "%d %c= %c%d", &idx0, &op, &var1, &idx1 ) != 4)
					error("comando invalido", line);
				printf("%d %c%d %c= %c%d\n", line, var0, idx0, op, var1, idx1);
				break;

			} 
			case 'i':{/* desvio condicional /
				char var0;
				int idx0, n1, n2;
				if(fscanf(myfp, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4)
					error("comando invalido", line);
				printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
				break;

			}
			case 'g':{/* desvio incondicional /
				int n1;
				if(fscanf(myfp, "o %d", &n1) != 1)
					error("comando invalido", line);
				printf("%d go %d\n", line, n1);
				break;
			}
			default: error("comando desconhecido", line);

		}
		line++;
		fscanf(myfp, " ");
	}*/
	
	return (funcp) codigo;

}

int main(/* int argc, char **argv*/){


	FILE *myfp;
	int res;
	funcp sb; 
	if((myfp = fopen("programa", "r")) == NULL){
		perror("nao conseguiu abrir arquivo");
		exit(1);

	}
	sb = geracod(myfp);
	res = (*sb)();
	printf(" retorno da funcao sb%d\n",res);
	return 0;
}


