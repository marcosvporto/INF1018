#include "geracod.h"
#include <stdlib.h>
#include <string.h>


#define MAX_BYTES 2000
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

void retorno_de_constante(unsigned char *codigo,int *byte_corrente, int constante){

	unsigned char v[4];
	preenche_cte (constante,v);
	memcpy(codigo+*byte_corrente,v,sizeof(v));
	*byte_corrente += sizeof(v);
	

}
void atribuicao(unsigned char *codigo,int *byte_corrente,char var0,int idx0,char var1, int idx1){
/*
 * var0 é o tipo da variavel que receberá o dado e pode ser v ou p
 * idx0 é o numero da variavel que receberá o dado e pode ser 1, 2, 3 ou 4
 * var1 é o tipo da variavel que contém o dado e pode ser v, p ou $
 * idx1 é o numero da variavel que contém o dado e pode ser qualquer numero
 */	unsigned char v[4];
	unsigned char cte2v[2] = {0xc7,0x45};
	if(var1 == '$'){// atribuição de constante
		
		preenche_cte(idx1,v);
		
		if(var0=='v'){// para uma variavel
			//preenche_cod(codigo,byte_corrente,cte2v,sizeof(cte2v));
			memcpy(codigo+*byte_corrente,cte2v,sizeof(cte2v));
			
			*byte_corrente+=sizeof(cte2v);
			printf(" pos_cod = %d\n",*byte_corrente);
 			if(idx0==1){//para v1
				codigo[*byte_corrente] = 0xfc;
				(*byte_corrente)++;
				printf(" pos_cod = %d\n",*byte_corrente);
			}
			else if(idx0==2){//para v2
				codigo[*byte_corrente] = 0xf8;
				(*byte_corrente)++;
			}
			else if(idx0==3){//para v3
				codigo[*byte_corrente] = 0xf4;
				(*byte_corrente)++;
			}
			else {//para v4
				codigo[*byte_corrente] = 0xf0;
				(*byte_corrente)++;
			}
		}
		else if(idx0==1){//para p1
			codigo[*byte_corrente] = 0xbf;
			(*byte_corrente)++;
		}else{//para p2
			codigo[*byte_corrente] = 0xbe;
			(*byte_corrente)++;
		}
		memcpy(codigo+*byte_corrente,v,sizeof(v));
		*byte_corrente += sizeof(v);
	}
	else if(var1=='p'){//de um parametro
		codigo[*byte_corrente] = 0x89;
		(*byte_corrente)++;
		if(var0=='v'){//para uma variavel
			codigo[*byte_corrente] = idx1==1?  0x7d : 0x75;// de p1 ou de p2?
			(*byte_corrente)++;
			if(idx0==1){//para v1
				codigo[*byte_corrente] = 0xfc;
				(*byte_corrente)++;
				printf(" pos_cod = %d\n",*byte_corrente);
			}
			else if(idx0==2){//para v2
				codigo[*byte_corrente] = 0xf8;
				(*byte_corrente)++;
			}
			else if(idx0==3){//para v3
				codigo[*byte_corrente] = 0xf4;
				(*byte_corrente)++;
			}
			else {//para v4
				codigo[*byte_corrente] = 0xf0;
				(*byte_corrente)++;
			}
		}
		else{//para outro parametro
			if(idx0 != idx1){
				codigo[*byte_corrente] = idx1==1? 0xfe :  0xf7;// de p1 ou de p2?
				(*byte_corrente)++;

			}
		}
	}
	else{//de uma variavel
		codigo[*byte_corrente] = 0x8b; //quando quando é atribuição entre um parametro e uma variavel só vai variar o primeiro byte 8b ou 89
		(*byte_corrente)++;//modificar o programa para para dimminuir o código caso dê tempo
		if(var0=='p'){// para um parametro
			codigo[*byte_corrente] = idx1==1? 0x7d :  0x75; // para p1 pu p2?
			(*byte_corrente)++;
			switch(idx1){// de qual variavel?
				case 1:{
					codigo[*byte_corrente] = 0xfc;
					(*byte_corrente)++;
					break;
				}
				case 2:{
					codigo[*byte_corrente] = 0xf8;
					(*byte_corrente)++;
					break;
				}
				case 3:{
					codigo[*byte_corrente] = 0xf4;
					(*byte_corrente)++;
					break;
				}
				case 4:{
					codigo[*byte_corrente] = 0xf0;
					(*byte_corrente)++;
					break;
				}
			}
		}
		else{//para outra variavel
			codigo[*byte_corrente] = 0x4d;	
			(*byte_corrente)++;
			switch(idx1){// de qual variavel?
				case 1:{
					codigo[*byte_corrente] = 0xfc;
					(*byte_corrente)++;
					break;
				}
				case 2:{
					codigo[*byte_corrente] = 0xf8;
					(*byte_corrente)++;
					break;
				}
				case 3:{
					codigo[*byte_corrente] = 0xf4;
					(*byte_corrente)++;
					break;
				}
				case 4:{
					codigo[*byte_corrente] = 0xf0;
					(*byte_corrente)++;
					break;
				}
			}
			codigo[*byte_corrente] = 0x89;
			(*byte_corrente)++;
			codigo[*byte_corrente] = 0x4d;	
			(*byte_corrente)++;
			if(idx0==1){//para v1
				codigo[*byte_corrente] = 0xfc;
				(*byte_corrente)++;
				printf(" pos_cod = %d\n",*byte_corrente);
			}
			else if(idx0==2){//para v2
				codigo[*byte_corrente] = 0xf8;
				(*byte_corrente)++;
			}
			else if(idx0==3){//para v3
				codigo[*byte_corrente] = 0xf4;
				(*byte_corrente)++;
			}
			else {//para v4
				codigo[*byte_corrente] = 0xf0;
				(*byte_corrente)++;
			}

		}
	}
}

funcp geracod(FILE *myfp){
	funcp f;
	unsigned char *codigo;
	

	
	
	unsigned char prologo[8] = {0x55,0x48,0x89,0xe5,0x48,0x83,0xec,0x10};
	unsigned char epilogo[2] = {0xc9,0xc3 };
	unsigned char retv[2]    = {0x8b,0x45};
	unsigned char retv1[1]   = {0xfc};
	unsigned char retv2[1]   = {0xf8};
	unsigned char retv3[1]   = {0xf4};
	unsigned char retv4[1]   = {0xf0};
	unsigned char retp[1]    = {0x89};
	unsigned char retp1[1]   = {0xf8};
	unsigned char retp2[1]   = {0xf0};
	unsigned char retcte[1]  = {0xb8};	
	char *linha = (char*)malloc(MAX_TAM_LINHA);
	int byte_corrente = 0;
	int line = 1;
	int c;
	codigo = (unsigned char*)malloc(MAX_BYTES);
	if(codigo==NULL){
		fprintf(stderr, "Falta de memória ao alocar bloco de codigo\n");
		exit(0);
	}	
	memcpy(codigo,prologo,sizeof(prologo));
	byte_corrente += sizeof(prologo);

	
	while ((c = fgetc(myfp)) != EOF ){
		
		switch(c){

			
			case 'r': {/* retorno */
				char var0;
				int idx0;
				
				if (fscanf (myfp, "et %c%d", &var0, &idx0) != 2)
					error("comando invalido",line);
				if(var0=='$'){//retorno de constante
					
					codigo[byte_corrente] = retcte[0];
					byte_corrente++;
					retorno_de_constante(codigo,&byte_corrente,idx0);
					
					
				}
				else if(var0=='v'){// retorno de variavel local
					memcpy(codigo+byte_corrente,retv,sizeof(retv));
					byte_corrente +=sizeof(retv);
					if(idx0==1){
						codigo[byte_corrente] = retv1[0];
						byte_corrente++;
					}
					else if(idx0==2){
						codigo[byte_corrente] = retv2[0];
						byte_corrente++;
					}
					else if(idx0==3){
						codigo[byte_corrente] = retv3[0];
						byte_corrente++;
					}
					else{
						codigo[byte_corrente] = retv4[0];
						byte_corrente++;
					}
				}
				else{//retorno de parametro
					codigo[byte_corrente] = retp[0];
					byte_corrente++;
					if(idx0==1){
						codigo[byte_corrente] = retp1[0];
						byte_corrente++;
					}
					else{
						codigo[byte_corrente] = retp2[0];
						byte_corrente++;
					}
				}	
				printf("%d ret %c%d\n", line,var0,idx0);
				memcpy(codigo+byte_corrente,epilogo,sizeof(epilogo));
				byte_corrente += sizeof(epilogo);
					
				break;
			}
			case 'v':
			case 'p':{/* atribuicao e op. aritmetica */
				char var0 = c, var1, op;
				int idx0, idx1;
				if(fscanf(myfp, "%d %c= %c%d", &idx0, &op, &var1, &idx1 ) != 4)
					error("comando invalido", line);
				switch(op){
					case ':':{printf("chegou\n");
						atribuicao(codigo,&byte_corrente,var0,idx0,var1,idx1);
						break;
					}
					case '*':{
						//multiplicacao(codigo,&byte_corrente,var0,idx0,var1,idx1);
						break;
					}
					case '+':{
						//soma(codigo,&byte_corrente,var0,idx0,var1,idx1);
						break;
					}
					case '-':{
						//subtracao(codigo,&byte_corrente,var0,idx0,var1,idx1);
						break;
					}

				}
				printf("%d %c%d %c= %c%d\n", line, var0, idx0, op, var1, idx1);
				
				break;

			} 
			case 'i':{/* desvio condicional */
				char var0;
				int idx0, n1, n2;
				if(fscanf(myfp, "f %c%d %d %d", &var0, &idx0, &n1, &n2) != 4)
					error("comando invalido", line);
				printf("%d if %c%d %d %d\n", line, var0, idx0, n1, n2);
				break;

			}
			case 'g':{/* desvio incondicional */
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
	}
	printf("dump de codigo\n");
	dump(codigo,byte_corrente);
	f = (funcp)codigo;
	return f;

}

int main(/* int argc, char **argv*/){


	FILE *myfp;
	int res=0;
	funcp sb; 
	if((myfp = fopen("programa", "r")) == NULL){
		perror("nao conseguiu abrir arquivo");
		exit(1);

	}
	sb = geracod(myfp);
	res = (*sb)(12,123);
	printf(" retorno da funcao sb = %d\n",res);
	return 0;
}


