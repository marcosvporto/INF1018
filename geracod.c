#include "geracod.h"
#include <stdlib.h>
#include <string.h>


#define MAX_BYTES 2000
#define MAX_TAM_INSTRUCAO 200
#define MAX_LINHAS 20


typedef struct tabela_goif{//tabela onde será gravada a ocorrencia de go ou ifs

	int poscod ; // posição no vetor de codigo onde se encontra o primeiro byte do go ou if
	int go; //flag para saber se é go ou if, se for go é 1, se for if é 0
	int linha_n1;//linha n para onde o go quer ir ou n1 do if
	int linha_n2;// n2 do if
	int linha_intr;//linha onde o if ou go é chamado 
	char var;// o tipo do dado que queremos analisar no if(parametro ou variavel)
	int id;// o numero do parameto ou variavel que queremos analisar no if

}Tabela_goif;


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
void preenche_if(unsigned char *codigo,Tabela_goif x, int * tam_instr){
	unsigned char *cod = &codigo[x.poscod];
	int distancia_n1=0,distancia_n2=0,i;
	
	cod[0]= 0x83;	
	if(x.var == 'p'){ //analisando parametro
		cod[1] = x.id == 1? 0xff : 0xfe;
		cod[2] = 0x00;
		unsigned char v[3] ={cod[0],cod[1],cod[2]}; 
	}
	else{ //analisando variavel
		cod[1] = 0x7d;
		cod[3] = 0x00;
		
		switch(x.id){
			case 1:{
				cod[2] = 0xfc;
				break;
			}
			case 2:{
				cod[2] = 0xf8;
				break;
			}
			case 3:{
				cod[2] = 0xf4;
				break;
			}
			case 4:{
				cod[2] = 0xf0;
				break;
			}
		}
		unsigned char v[4] ={cod[0],cod[1],cod[2],cod[3]};
	}
	// se for je pra perto x[4] = 74 se for pra longe x[4] = 0f e x[5] = 84
	if(x.linha_n2>x.linha_intr){// n2 está depois do if
		for(i=x.linha_intr;i<x.linha_n2-1;i++){
			distancia_n2+=tam_instr[i];
		}
		if(distancia<127){// short je positivo
			cod[4] = 0x74;
			cod[5] = (unsigned char) distancia;
		}
		else{// long je positivo
			cod[4] = 0x0f;
			cod[5] = 0x84;
			cod[6] = (unsigned char) distancia;
			cod[7] = 0x00;
			cod[8] = 0x00;
			cod[9] = 0x00;
		}
	}
	else{//n2 está antes do if
		for(i=x.linha_n2-1;i<x.linha_intr-1;i++){
			distancia_n2+=tam_intr[i];
		}
	}
	memcpy(cod+10,v,sizeof(v));
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
void desvio_incondicional(unsigned char *cod, int line,int *tamanho_instrucao){
	int distancia = 0;
	int linha_desvio = (int) *(cod+1);
	int i;
	unsigned char shrt[3]= {0x01,0xc3,0xeb};
	unsigned char lngneg[3]={0xff,0xff,0xff};
	unsigned char lngpos[3]={0x00,0x00,0x00};
	unsigned char v[4];
	if(linha_desvio>line){
		
		printf("line = %d e linha desvio = %d\n",line,linha_desvio);
		for(i=line;i<linha_desvio-1;i++){// jump pra frente(positivo)
			distancia += tamanho_instrucao[i];
			
		}
		if(distancia <= 126){// short jump
			//printf(" short jmp positivo\n");
			memcpy(cod+1,shrt,sizeof(shrt));
			*(cod+4) = (unsigned char) distancia;
		}
		else if(distancia){//long jump
			//printf(" long jmp positivo\n");
			*cod = 0xe9;
			
			*(cod+1) = (unsigned char) distancia;
			memcpy(cod+2,lngpos,sizeof(lngpos));  
		}
	}
	else{
		for(i=linha_desvio-1;i<line-1;i++){//jump para tras(negativo)
			distancia += tamanho_instrucao[i];
			printf("distancia+= %d\n",tamanho_instrucao[i]);
		}
		distancia++;//contando o 0xeb
		printf("distancia final = %d\n",distancia);
		if(distancia <= 127){// short jump
			distancia+=3;
			printf(" short jmp negativo\n");
			memcpy(cod+1,shrt,sizeof(shrt));
			*(cod+4) = (unsigned char) 255-distancia;
		}
		else if(distancia){
			//printf(" long jmp negativo\n");
			*cod = 0xe9;
			*(cod+1) = (unsigned char) 125 - (distancia - 127); 
			memcpy(cod+2,lngneg,sizeof(lngneg)); 
		}
	}
	//printf("distancia = %d",distancia);	
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
			//printf(" pos_cod = %d\n",*byte_corrente);
 			if(idx0==1){//para v1
				codigo[*byte_corrente] = 0xfc;
				(*byte_corrente)++;
				//printf(" pos_cod = %d\n",*byte_corrente);
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
	unsigned char *codigo,*cod;
	int *tamanho_instrucao;
	Tabela_goif *tbl;
	unsigned char prologo[8]  = {0x55,0x48,0x89,0xe5,0x48,0x83,0xec,0x10};
	unsigned char epilogo[2]  = {0xc9,0xc3 };
	unsigned char desvinc[3]  = {0x00,0x00,0x00};
	unsigned char govazio[5]  = {0x00,0x00,0x00,0x00,0x00,};
	unsigned char ifvazio[21];
	unsigned char retv[2]     = {0x8b,0x45};
	unsigned char retv1[1]    = {0xfc};
	unsigned char retv2[1]    = {0xf8};
	unsigned char retv3[1]    = {0xf4};
	unsigned char retv4[1]    = {0xf0};
	unsigned char retp[1]     = {0x89};
	unsigned char retp1[1]    = {0xf8};
	unsigned char retp2[1]    = {0xf0};
	unsigned char retcte[1]   = {0xb8};	
	int byte_corrente = 0;
	int line = 1;
	int c;
	int i;
	int goifs=0;
	codigo = (unsigned char*)malloc(MAX_BYTES);
	tamanho_instrucao = (int*)malloc(MAX_LINHAS);
	tbl = (Tabela_goif*)malloc(MAX_LINHAS*sizeof(Tabela_goif));
	if(codigo==NULL){
		fprintf(stderr, "Falta de memória ao alocar bloco de codigo\n");
		exit(0);
	}
	if(tamanho_instrucao==NULL){
		fprintf(stderr, "Falta de memória ao alocar tamanho das intruçoes\n");
		exit(0);
	}
	for(i=0;i<21;i++){
		ifvazio[i] = 0x00;
	}
	for(i=0;i<MAX_LINHAS;i++){
		tbl[i].poscod = 0;
	}
	memcpy(codigo,prologo,sizeof(prologo));
	byte_corrente += sizeof(prologo);

	
	while ( (c = fgetc(myfp)) != EOF ){
		printf("byte_inicial = %d\n",byte_corrente);
		int byte_inicial = byte_corrente;
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
					case ':':{
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
				memcpy(codigo+byte_corrente,ifvazio,sizeof(ifvazio));
				//tbl[goifs] = {byte_corrente,0,n1,n2,line,var0,idx0};
				tbl[goifs].poscod = byte_corrente;
				tbl[goifs].go = 0;
				tbl[goifs].linha_n1 = n1;
				tbl[goifs].linha_n2 = n2;
				tbl[goifs].linha_intr=line;
				tbl[goifs].var = var0;
				tbl[goifs].id = idx0;				
				goifs++;
				break;

			}
			case 'g':{/* desvio incondicional */
				int n1;
				if(fscanf(myfp, "o %d", &n1) != 1)
					error("comando invalido", line);
				codigo[byte_corrente] = 0xeb;
				byte_corrente++;
				codigo[byte_corrente] = (unsigned char) n1;
				byte_corrente++;
				memcpy(codigo+byte_corrente,desvinc,sizeof(desvinc));
				byte_corrente += sizeof(desvinc);
				printf("%d go %d\n", line, n1);
				break;
			}
			default: error("comando desconhecido", line);

		}
		tamanho_instrucao[line-1] = byte_corrente-byte_inicial;
		printf("tamanho_instrucao[%d] = %d\n",line-1,byte_corrente-byte_inicial);
		line++;
		fscanf(myfp, " ");
	}
	cod = codigo+sizeof(prologo);
	
	for(i=0;i<line;i++){
	//printf("testeeeeeeeeeeeeeeeeeeeeeeeee :%02x\n",cod[i]);
		if(cod[i]== 0xeb){
			//printf("testeeeeeeeeeeeeeeeeeeeeeeeee :%02x\n",cod[i]);
			desvio_incondicional(&cod[i],i+1,tamanho_instrucao);	
		}
		cod+=(tamanho_instrucao[i]-1);
	}
	for(i=0;i<goifs;i++){
		if(tbl[i].poscod!=0){
			printf("[%d,%d,%d,%d,%d,%c,%d\n",tbl[i].poscod,tbl[i].go,tbl[i].linha_n1,tbl[i].linha_n2,tbl[i].linha_intr,tbl[i].var,tbl[i].id);
			if(tbl[i].go == 0){
				preenche_if(codigo,tbl[i],tamanho_instrucao);
			}
			else{
				//preenche_go(codigo,tbl[i].poscod,tbl[i].linha_intr,tbl[i].linha_n1);
			}
		}
	}
	printf("dump de codigo :%d\n",byte_corrente);
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


