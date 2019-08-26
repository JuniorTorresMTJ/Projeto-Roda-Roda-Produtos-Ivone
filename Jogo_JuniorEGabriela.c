#include <stdio.h>                 //Biblioteca padrão de C.
#include <math.h>                  //Biblioteca de matemática.
#include <string.h>                //Biblioteca para maniputação de cadeias de caracteres.
#include <locale.h>                //Biblioteca localiza o programa quando fazemos a adaptação às características de um determinado idioma.
#include <stdbool.h>               //Biblioteca que manipula variáveis lógicas, como verdadeiro (=1) ou falso (=0).
#include <windows.h>               //Restringe para Windows
#include <conio.h>				   //Biblioteca para usar funções, como : coodernadas.


int main(void);
int menu(void);
int loadDebug(void);				//Função de debug, imprime conteúdo de palavra.dat, de premios.dat e os nomes dos jogadores.
int rodada(void);
int sorteio(void);

char palavras[5][4][17];			//Matriz onde 'palavras.dat' é carregado. 0=nome da dica, 1+=palavra.
char premios[12][17];				//Matriz onde 'premios.dat' é carregado. Contém todos os prêmios.
int id[5];							//Vetor que armazena a quantidade de palavras de cada dica.
char jogador[3][11];				//Matriz que armazena o nome dos jogadores.
int  cash[3];						//Vetor que armazena a pontuação dos jogadores.
char dica[4][17];					//Matriz que, após sorteio da dica, armazena as palavras em `tracinhos` e letras certas.
char menu_opt;						//Variavel para opção do menu.
bool leave = false;					//Booleano true e false.
bool primeiraRodada = true;      
int endgame = 0;
int vezJogador = 0;
int randomDica;
int randomPremio;
int premioMoney;
char letrasUsadas[24];
bool tenteDenovo = false;
int dump[256];

FILE * fword;
FILE * fprize;

int loadsrc(char vetor1[][4][17], char vetor2[][17], int id[]);					    	//Carrega PALAVRAS.DAT e PREMIOS.DAT.
int resultado(int p1, int p2, int p3);											    	//Recebe o total dos 3 jogadores, verifica o maior e retorna os jogadores vitoriosos.
int comparar(char v_in[][4][17], char v_out[][17], char letraInput, int dica);	     	//Pesquisa entre as palavras no jogo pela da letra escolhida. word_buf[] será modificada de acordo.
int pcomparar(char v_in[][4][17], char v_out[][17], char palavra[][17], int dica, int id);
int contador_(char v_out[][17]);

int main() {
	int i, x;
	setlocale(LC_ALL, "Portuguese");	//Define os caracteres válidos. "Portuguese" permite o uso livre de letras acentuadas, como na língua Portuguesa.
	loadsrc(palavras, premios, id);     //Carrega os arquivos criados
	while (menu() != 1) {				//Menu retornará 0 se uma seleção do usuário for necessária. 1 quebra o loop, 2 sairá do programa.
		if (leave) return 0;
		else menu_opt = getch();
	}
	for (i=0; i<3; i++) {                           //Nome dos Jogadores.
		printf("\n\t JOGAMONA %d >> ", i+1);
		fgets(jogador[i], 11, stdin);
		jogador[i][strcspn(jogador[i], "\n")] = '\0';
		fflush(stdin);
	}

	srand(time(NULL));			  //Controla a escolha da 'seed' (algorítimo gerador de número pseudo-aleatório). Se seed permanecer igual, a sequência será repetida para cada execução do programa.
	randomDica = rand() % 4;	 // Sorteio da dica.

	primeiraRodada = true;
	vezJogador = 0;
	endgame = 0;
	cash[0] = 0; cash[1] = 0; cash[2] = 0; 

	while (x!=1){							// Se igual a 1, acaba o jogo.
		if (vezJogador == 3)				// Se igual a 0, continua cada rodada.
			vezJogador = 0;
		x = rodada();                      //Imprime tela do jogo e continua rodando enquanto não retornar 1. 
		vezJogador++;						
	}
	menu_opt = 0;
	main();
	return 0;
}

int loadsrc(char vetor1[][4][17], char vetor2[][17], int id[]) {
	int i, j;
	char buffer[17];														    //O vetor guarda a leitura atual antes de enviar à matriz, para verificação.

	fword = fopen("src\\palavras.dat", "r");                                   //Carrega dados dentro do arquivo. Ao detectar um @, o leitor avança para a próxima linha da matriz.
	fprize = fopen("src\\premios.dat", "r");
	for (i=0; i<5; i++) {
		id[i] = -1;
		for (j=0; j<5; j++) {
			fscanf(fword, "%s", buffer);
			if (strcmp(buffer, "@") == 0) {
				break;
			}
			strcpy(vetor1[i][j], buffer);
			id[i]++;
		}
	}

	for (i=0; i<12; i++) {                                                    //Carrega dados dentro do arquivo.
		fscanf(fprize, "%s", buffer);
		strcpy(vetor2[i], buffer);
	}
	return 0;
}

int resultado(int p1, int p2, int p3) {
	if (p1 == p2 && p1 == p3) return 123;
	else if (p1 == p2 && p1 > p3) return 12;
	else if (p1 == p3 && p1 > p2) return 13;
	else if (p2 == p3 && p2 > p1) return 23;
	else if (p1 > p2 && p1 > p3) return 1;
	else if (p2 > p1 && p2 > p3) return 2;
	else return 3;
}

int comparar(char v_in[][4][17], char v_out[][17], char letraInput, int dica) {   //Ex: comparar(palavras, dica, <char da letra pedida>, <pista sorteada>); 0=correto, 1=errado, 2=já usado.
	bool correto;
	int j, k;
	int bonus = 0;
	correto = false;
	for (j=1; j<4; j++) {
		for (k=0; k<strlen(v_in[dica][j]); k++) {                                 //Mostra a quantidade de caracteres usados até chegar o '\0'.
			if (v_in[dica][j][k] == letraInput) {
				if (v_out[j][k] == letraInput) {
					return -1;
				}
				v_out[j][k] = letraInput;
				correto = true;
				bonus++;
			}
		}
	}
	if (correto) {
		return bonus;
	} else {
		return 0;
	}
}

int pcomparar(char v_in[][4][17], char v_out[][17], char palavra[][17], int dica, int id) {
	int correto;
	int i, j;
	correto = 0;
	for (i=0; i<id; i++) {
		for (j=1; j<4; j++) {
			if (strcmp(palavra[i], v_in[dica][j]) == 0) {
				correto++;
			}
		}
	}
	if (correto == id) {
		for (i=0; i<id; i++) {
			for (j=1; j<4; j++) {
				if (strcmp(palavra[i], v_in[dica][j]) == 0) {
					strcpy(v_out[j], palavra[i]);
				}
			}
		}
		return 1;
	} else {
		return 0;
	}
}

int contador_(char v_out[][17]){		                                             //Verifica a quantidade de tracinhos restantes no jogo.
	int count=0;
	int j, k;
	for (j=1; j<4; j++) {
		for (k=0; k<strlen(v_out[j]); k++) {
			if (v_out[j][k] == '_') count++;
		}
	}
	return count;
}

int gotoxy(int column, int line) {
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(GetStdHandle( STD_OUTPUT_HANDLE ), coord);
	return 0;
}

int menu() {
	system("cls");					      //'menu_opt' exibirá uma tela para cada opção escolhida. Ao retornar das outras, menu_opt = 0 para exibir default.
	switch(menu_opt) {                    //Menu do Jogo. 
		case '1': {
			system("cls");
			printf("\n\t\t\t       Começar Jogo"
				   "\n\t\t\t     ----------------"
				   "\n\n     Digite o nome dos três participantes (máximo de 10 letras, sem acentos).\n");
			return 1;
		} break;
		case '2': {
			printf("\n\t\t\t\t Instruções"
				   "\n\t\t\t\t-------------"
				   "\n\n     O objetivo do jogo é acertar as palavras sorteadas, utilizando a dica dada. Modo de jogar:"
				   "\n\n\t1 -> Gire a roda de prêmios \n\t2 -> Chute uma letra"
				   "\n\n     Se a letra fizer parte da palavra, você ganhará a quantia sorteada \n     na roda. Mas tome cuidado: a roda não tem apenas prêmios. Há\n     também as opções 'Passe a Vez' e 'Perde Tudo'"
				   "\n\n     Ao final do jogo, o jogador que acertar as palavras vence.\n     Porém, isto é apenas um joguinho, então você não irá \n     receber o MONEY MONEY, HONEY. VAI TRABALHAR E SASHAY AWAY");   
			printf("\n\n\n\t\t\t      Vocabulário dos Memes"
				   "\n\t\t\t     ------------------------"
				   "\n\n\tHITAR-> Conseguir, dar certo \n\n\tPRODUTOS IVONE-> Sério? Nunca assistiu 'Todo Mundo Odeia o Chris'? \n\n\tSASHAY AWAY-> Ir embora \n\n\tCONDRAGULATIONS -> 'Meus parabéns'"
				   "\n\n\tJOGAMONA-> Jogadoras \n\n\tJÁ ACABOU, JÉSSICA?-> Meme que estourou em 2015. Sério? Você vive em uma caverna, por acaso? \n\n\tPARÇA-> Parceiro"
				   "\n\n\tBETINA-> Meme de 2019, de uma menina de muito sucesso na vida. Ou, também, o apelido da Larissa na aula da Jocelinda \n\n\tTHANK YOU, NEXT-> Vai ouvir o hino da Ariana Grande, seu suíno sem cultura" 
				   "\n\n\tERRADO, INFERNO-> Fala da fada sem defeitos, cristal lapidado da Prof. Jocelinda");
			printf("\n\n\n\t\t\t\t ->> Pressione qualquer tecla para voltar.");
			return 0;
		} break;
		case '3': {
			leave = true;
			return 0;
		} break;
		default: {
			printf("\n\n\t           RODA A RODA PRODUTOS IVONE S2");
			printf("\n\n\t      S2-------------------------------------S2");
			printf("\n\n\t                Selecione uma opção:");
			printf("\n\n\t      S2-------------------------------------S2"
				   "\n\n\t\t\t    1. Jogar\n\t\t\t    2. Instruções\n\t\t\t    3. Sair");
		} break;
	}
}
int loadDebug() {                                                                           	//Função debug para ver se não há erros.
	int i;
 	printf("\n%s %s %s", palavras[0][0], palavras[0][1], palavras[0][2]);
	printf("\n%s %s %s %s", palavras[1][0], palavras[1][1], palavras[1][2], palavras[1][3]);
	printf("\n%s %s %s %s", palavras[2][0], palavras[2][1], palavras[2][2], palavras[2][3]);
	printf("\n%s %s", palavras[3][0], palavras[3][1]);
	printf("\n%s %s %s\n\n", palavras[4][0], palavras[4][1], palavras[4][2]);
	for (i=0; i<12; i++) {
		printf("%s ", premios[i]);
	}
	printf("\n\n");
	for (i=0; i<3; i++) {
		printf("%s ", jogador[i]);
	}
	getch();
}

int rodada() {                                   
	char letra;																					//Variáveis locais.
	char palavraEnd[3][17];
	int i,j;
	int cmp;
	int progresso;
	int result;
	system("cls");

	gotoxy(18,3); printf("-- A diquinha é: %s --\n\n", palavras[randomDica][0]);               //Sorteia o money.
	if (primeiraRodada == true){	                                                            //Caso for a primeira vez jogando, então só sorteia entre 0 a 9 e carrega os tracinhos.
		for(i=1;i<4;i++){
			printf("\n\t\t\t     ");
			for (j=0; j<strlen(palavras[randomDica][i]); j++) {                               
				dica[i][j] = '_';
				printf("%c ", dica[i][j]);
			}
		}
	} else {									                                               //Senão sorteia entre 0 a 11 e printa os tracinhos e o que já acertaram
		for(i=1;i<4;i++) {
			printf("\n\t\t\t     ");
			for (j=0; j<strlen(palavras[randomDica][i]); j++) {
				printf("%c ", dica[i][j]);
			}
		}
	}
	progresso = contador_(dica);
	switch (vezJogador) {
		case 0: gotoxy(4,11); break;                                  //Esse comando serve para posicionar qualquer texto na tela de saída do programa.
		case 1: gotoxy(32,11); break;
		case 2: gotoxy(60,11); break;
	}
	printf("       \\/");
	gotoxy(4,13);printf("[P1] %s",jogador[0]);  gotoxy(32,13);printf("[P2] %s",jogador[1]);   gotoxy(60,13);printf("[P3] %s",jogador[2]);
	gotoxy(4,14);printf("=--------------=");    gotoxy(32,14);printf("=--------------=");     gotoxy(60,14);printf("=--------------=");
	gotoxy(4,15);printf("R$ %d,00",cash[0]);   gotoxy(32,15);printf("R$ %d,00",cash[1]);    gotoxy(60,15);printf("R$ %d,00",cash[2]);
	printf("\n\n");

	if (progresso <= 3 && progresso > 0 && endgame == 0) {            // Final do jogo
		endgame = 1;
		gotoxy(0,18);
		gotoxy(36,19); printf("--RODADA ESPECIAL--");
		gotoxy(36,20); printf("                   ");
		gotoxy(36,21); printf("Consegue adivinhar a palavra?");
		gotoxy(36,22);
		for (i=5; i>0; i--) {
			printf("%d! ", i);
			Sleep(1000);                                             //Serve para parar um programa por determinado tempo.
		}
		gotoxy(36,19); printf("                                ");
		gotoxy(36,20); printf("                                ");
		gotoxy(36,21); printf("                                ");
		gotoxy(36,22); printf("                                ");
	} else if (progresso == 0) {
		gotoxy(0,18);
		gotoxy(36,19); printf("--GAME OVER--");
		gotoxy(36,20); printf("                ");
		gotoxy(4,11); printf("                                                                   ");
		Sleep(2000);
		gotoxy(36,21);
		if (endgame == 1) {
			result = vezJogador;
		} else {
			result = resultado(cash[0], cash[1], cash[2]);
		}
		switch (result) {                                          //'Printação' de resultados.
			case 0: {
				gotoxy(36,21); printf("CONDRAGULATIONS, ARRASOU VIADO: [P%i] %s", 3, jogador[3]);
			} break;
			case 1: case 2: case 3: {
				gotoxy(36,21); printf("CONDRAGULATIONS, ARRASOU VIADO: [P%i] %s", result, jogador[result-1]);
			} break;
			case 12: printf("POOXA, OS JOGARORES %s e %s EMPATARAM", jogador[0], jogador[1]); break;
			case 13: printf("POOXA, OS JOGARORES %s e %s EMPATARAM", jogador[0], jogador[2]); break;
			case 23: printf("POOXA, OS JOGARORES %s e %s EMPATARAM", jogador[1], jogador[2]); break;
			case 123: printf("POXA, NÃO HITOU. TODOS PERDERAM"); break;
		}
		gotoxy(36,22); printf(">> Sair");
		getch();
		return 1;
	}


	if (tenteDenovo == true) {
		gotoxy(0,18);
	}
	else{
		sorteio();
	}
	tenteDenovo = false;

	if (randomPremio < 10 && endgame == 0) {
		premioMoney = strtol(premios[randomPremio], NULL, 10);                     //Converte a parte inicial da string em str para um valor int longo de acordo com a base especificada.
		gotoxy(36,19); printf("Valendo R$ %d.00",premioMoney);
		gotoxy(36,20); printf("Escolha uma letra: "); 
		letra = getch(); letra = toupper(letra); printf("%c",letra);
		
		//if (verificaLetra(letra) == 1)
		//	gotoxy(15,20);printf("Letra já usada");
		cmp = comparar(palavras, dica, letra, randomDica);                       //Compara e retorna um inteiro.
		if (cmp > 0) {
			cash[vezJogador] = cash[vezJogador] + (premioMoney * cmp);
			
			gotoxy(36,22); printf("--ACERTOU, BETINA!!--");
			Sleep(2500);
			return 0;
		} else if (cmp == -1) {
			gotoxy(36,22); printf("Ô INFEEERNO, A LETRA JÁ FOI USADA");
			vezJogador--;
			Sleep(2500);
			tenteDenovo = true;
			return 0;
		} else {
			gotoxy(36,22); printf("--ERRAAAAAADO INFERNO!!!!!--");
			Sleep(2500);
			return 0;
		}
	} else if (randomPremio < 10 && endgame == 1) {
		premioMoney = strtol(premios[randomPremio], NULL, 10); 
		gotoxy(36,19); printf("Valendo R$ %d.00",premioMoney);
		for (i=0; i<id[randomDica]; i++) {
			gotoxy(36, 20+i);
			printf("Qual a %dº palavra? >> ", i+1);
			fgets(palavraEnd[i], 17, stdin);
			palavraEnd[i][strcspn(palavraEnd[i], "\n")] = '\0';
			for (j=0; j<strlen(palavraEnd[i]); j++) {
				palavraEnd[i][j] = toupper(palavraEnd[i][j]);
			}
		}
		
		cmp = pcomparar(palavras, dica, palavraEnd, randomDica, id[randomDica]);
		if (cmp == 1) {
			cash[vezJogador] = cash[vezJogador] + premioMoney;
			gotoxy(36,23); printf("--HITOOOOOOOOOOOOUUU, PARÇA!!!!!-");
			Sleep(2500);
			return 0;
		} else if (cmp == -1) {
			gotoxy(36,23); printf("A PALAVRA JÁ ACABOU, JÉSSICA");
			vezJogador--;
			Sleep(2500);
			tenteDenovo = true;
			return 0;
		} else {
			gotoxy(36,23); printf("--ERROU, INFEEEEEEEEEEERNO!!!!--");
			Sleep(2500);
			return 0;
		}
	} else if (randomPremio == 10) {
		gotoxy(36,20); printf("--THANK YOU, NEXT--");
		Sleep(2500);
		return 0;
	} else if (randomPremio == 11) {
		cash[vezJogador] = 0;
		gotoxy(36,20); printf("--JÁ ERA, BRO. PERDEU TUDO--");
		Sleep(2500);
		return 0;
	}
	return 1;
}

int sorteio() {
	int spin;
	if (primeiraRodada) {
		randomPremio = rand() % 10;
		primeiraRodada = false;				                          //Define primeiraRodada como false, indicando que não é a primeira vez jogando.
	} 
	else randomPremio = rand() % 12;
	for (spin=0; spin<13; spin++) {
		randomPremio++;
		if (randomPremio == 12) randomPremio = 0;
		if (spin % 2 == 0) {
			gotoxy(0,18);
		} else {
			gotoxy(0,18);
		}
		gotoxy(36,19); printf("Rodando...",premioMoney);
		gotoxy(36,20);
		if (randomPremio == 10) printf("PASSE A VEZ ");
		else if (randomPremio == 11) printf("PERDE TUDO  ");
		else printf("R$ %s,00    ", premios[randomPremio]);
		Sleep(100);
	}
}
