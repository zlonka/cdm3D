// cdm4Denum.cpp : compte le nombre de grilles possibles de N coups.
//

/*
results: 
*/
#include "stdafx.h"
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

clock_t  t1, t2;

#define E 1
#define SE 2
#define S 4
#define SW 8
#define ON 16
#define DEPART 32

/*
#define WIDTH 18
#define HEIGHT 18
#define W0 6
#define H0 6
*/
#define WIDTH 16
#define HEIGHT 16
#define W0 (WIDTH/2)
#define H0 (HEIGHT/2)

int levelMin = 32;
char fout[256];

// 16=on, 1=E, 2=SE, 4=S, 8=SW
#define PT unsigned char
PT grid[HEIGHT][WIDTH];
PT gridDepart[HEIGHT][WIDTH];
char td[4][4];

void dump(bool tofile = true);

typedef struct{
	PT grille[HEIGHT][WIDTH];
}DATA;

int nDdata = 0;

#define VERSION_T 0

#if VERSION_T
void trace(const char x, const char y, const char d, PT *ptr){
	switch (d){
	case 0:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | E);
		ptr[1] = grid[y][x + 1];	grid[y][x + 1] |= (ON | E);
		ptr[2] = grid[y][x + 2];	grid[y][x + 2] |= (ON);
		break;
	case 1:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | SE);
		ptr[1] = grid[y + 1][x + 1];	grid[y + 1][x + 1] |= (ON | SE);
		ptr[2] = grid[y + 2][x + 2];	grid[y + 2][x + 2] |= (ON);
		break;
	case 2:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | S);
		ptr[1] = grid[y + 1][x];	grid[y + 1][x] |= (ON | S);
		ptr[2] = grid[y + 2][x];	grid[y + 2][x] |= (ON);
		break;
	case 3:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | SW);
		ptr[1] = grid[y + 1][x - 1];	grid[y + 1][x - 1] |= (ON | SW);
		ptr[2] = grid[y + 2][x - 2];	grid[y + 2][x - 2] |= (ON);
		break;
	}
}
#else
void trace(const char x, const char y, const char d, PT *ptr){
	switch (d){
	case 0:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | E);
		ptr[1] = grid[y][x + 1];	grid[y][x + 1] |= (ON | E);
		ptr[2] = grid[y][x + 2];	grid[y][x + 2] |= (ON | E);
		break;
	case 1:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | SE);
		ptr[1] = grid[y + 1][x + 1];	grid[y + 1][x + 1] |= (ON | SE);
		ptr[2] = grid[y + 2][x + 2];	grid[y + 2][x + 2] |= (ON | SE);
		break;
	case 2:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | S);
		ptr[1] = grid[y + 1][x];	grid[y + 1][x] |= (ON | S);
		ptr[2] = grid[y + 2][x];	grid[y + 2][x] |= (ON | S);
		break;
	case 3:
		ptr[0] = grid[y][x];		grid[y][x] |= (ON | SW);
		ptr[1] = grid[y + 1][x - 1];	grid[y + 1][x - 1] |= (ON | SW);
		ptr[2] = grid[y + 2][x - 2];	grid[y + 2][x - 2] |= (ON | SW);
		break;
	}
}
#endif

void untrace(const char x, const char y, const char d, const PT *ptr){
	grid[y][x] = ptr[0];
	switch (d){
	case 0:
		grid[y][x + 1] = ptr[1];
		grid[y][x + 2] = ptr[2];
		break;
	case 1:
		grid[y + 1][x + 1] = ptr[1];
		grid[y + 2][x + 2] = ptr[2];
		break;
	case 2:
		grid[y + 1][x] = ptr[1];
		grid[y + 2][x] = ptr[2];
		break;
	case 3:
		grid[y + 1][x - 1] = ptr[1];
		grid[y + 2][x - 2] = ptr[2];
		break;
	}
}

void ini(){
	int i, x = W0, y = H0;
	for (i = 0; i<HEIGHT; i++){
		for (int j = 0; j<WIDTH; j++){
			gridDepart[i][j] = grid[i][j] = 0;
		}
	}
	gridDepart[y][x] = grid[y][x] = ON | DEPART;
	gridDepart[y - 1][x] = grid[y - 1][x] = ON | DEPART;
	gridDepart[y + 1][x] = grid[y + 1][x] = ON | DEPART;
	gridDepart[y][x - 1] = grid[y][x - 1] = ON | DEPART;
	gridDepart[y][x + 1] = grid[y][x + 1] = ON | DEPART;
}
typedef struct{
	char j, i, d;
}SOL;

int profMax = 0;
unsigned long long configMax = 0LL;
unsigned long long config = 0LL;


void dump(bool tofile){
	int cc = 250;
	printf("levelMin=%d\n", levelMin);
	for (int j = 0; j<HEIGHT; j++){
		for (int i = 0; i<WIDTH; i++){
			// if (grid[j][i]&ON) printf("o[%d] ",grid[j][i]); else printf("  ");
			if (grid[j][i] & DEPART) printf("O"); else if (grid[j][i] & ON) printf("+"); else printf("%c", cc);
		}
		printf("\n");
	}
	if (tofile){
		FILE *f = fopen(fout, "a");
		for (int j = 0; j < HEIGHT; j++){
			for (int i = 0; i < WIDTH; i++){
				if (grid[j][i] & DEPART) fprintf(f,"O"); else if (grid[j][i] & ON) fprintf(f,"+"); else fprintf(f,"%c", '.');
			}
			fprintf(f, "\n");
		}
		fprintf(f, "\n");
		fclose(f);
	}
}

int nbDep = 0;

typedef struct{
	char x, y, d;
}COUP;

COUP coup[256];

int canTrace(char i, char j, char d){
	// if ((grid[j][i] & ON) != 0) return -1;
	if ((grid[j - 1][i - 1] & ON) == 0 && (grid[j - 1][i] & ON) == 0 && (grid[j - 1][i + 1] & ON) == 0
		&& (grid[j][i - 1] & ON) == 0 && (grid[j][i + 1] & ON) == 0
		&& (grid[j + 1][i - 1] & ON) == 0 && (grid[j + 1][i] & ON) == 0 && (grid[j + 1][i + 1] & ON) == 0
		){
		// printf("pas de voisin\n");
		return -2;	// pas de 8-voisins on arrete 
	}
	// E 0
	if ((grid[j][i - 2] & ON) && (grid[j][i - 1] & ON)){	// o o x
		if ((grid[j][i - 2] & E) == 0 && (grid[j][i - 1] & E) == 0){
			return 1;
		}
	}
	if ((grid[j][i - 1] & ON) && (grid[j][i + 1] & ON)){	// o x o
		if (/*(grid[j][i - 2] & E) == 0 && */(grid[j][i - 1] & E) == 0 && (grid[j][i + 1] & E) == 0/* && (grid[j][i +2] & E) == 0*/){
			return 2;
		}
	}
	if ((grid[j][i + 1] & ON) && (grid[j][i + 2] & ON)){	// x o o
		if ((grid[j][i - 1] & E) == 0 && (grid[j][i + 1] & E) == 0 && (grid[j][i + 2] & E) == 0){
			return 3;
		}
	}

	// SE 1
	if ((grid[j - 2][i - 2] & ON) && (grid[j - 1][i - 1] & ON)){	// o o x
		if ((grid[j - 2][i - 2] & SE) == 0 && (grid[j - 1][i - 1] & SE) == 0){
			return 4;
		}
	}
	if ((grid[j - 1][i - 1] & ON) && (grid[j + 1][i + 1] & ON)){	// o x o
		if (/*(grid[j-2][i - 2] & SE) == 0 &&*/ (grid[j - 1][i - 1] & SE) == 0 && (grid[j + 1][i + 1] & SE) == 0){
			return 5;
		}
	}
	if ((grid[j + 1][i + 1] & ON) && (grid[j + 2][i + 2] & ON)){	// x o o
		if ((grid[j - 1][i - 1] & SE) == 0 && (grid[j + 1][i + 1] & SE) == 0 && (grid[j + 2][i + 2] & SE) == 0){
			return 6;
		}
	}
	// S 2
	if ((grid[j - 2][i] & ON) && (grid[j - 1][i] & ON)){	// o o x
		if ((grid[j - 2][i] & S) == 0 && (grid[j - 1][i] & S) == 0){
			return 7;
		}
	}
	if ((grid[j - 1][i] & ON) && (grid[j + 1][i] & ON)){	// o x o
		if (/*(grid[j - 2][i] & S) == 0 && */(grid[j - 1][i] & S) == 0 && (grid[j + 1][i] & S) == 0){
			return 8;
		}
	}
	if ((grid[j + 1][i] & ON) && (grid[j + 2][i] & ON)){	// x o o
		if ((grid[j - 1][i] & S) == 0 && (grid[j + 1][i] & S) == 0 && (grid[j + 2][i] & S) == 0){
			return 9;
		}
	}
	// SW
	if ((grid[j - 2][i + 2] & ON) && (grid[j - 1][i + 1] & ON)){	// o o x
		if ((grid[j - 2][i + 2] & SW) == 0 && (grid[j - 1][i + 1] & SW) == 0){
			return 10;
		}
	}
	if ((grid[j - 1][i + 1] & ON) && (grid[j + 1][i - 1] & ON)){	// o x o
		if (/*(grid[j - 2][i+2] & SW) == 0 && */(grid[j - 1][i + 1] & SW) == 0 && (grid[j + 1][i - 1] & SW) == 0){
			return 11;
		}
	}
	if ((grid[j + 1][i - 1] & ON) && (grid[j + 2][i - 2] & ON)){	// x o o
		if ((grid[j - 1][i + 1] & SW) == 0 && (grid[j + 1][i - 1] & SW) == 0 && (grid[j + 2][i - 2] & SW) == 0){
			return 12;
		}
	}
	return -3;
}
void playRand(int level){
	if (level > levelMin) return;
	COUP possib[256];
	int nPossib = 0;
	for (char j = 2; j<HEIGHT-2; j++){
		for (char i = 2; i<WIDTH - 2; i++){
			if ((grid[j][i] & ON) != 0) continue;
			// E 0
			if ((grid[j][i - 2] & ON) && (grid[j][i - 1] & ON)){	// o o x
				if ((grid[j][i - 2] & E) == 0 && (grid[j][i - 1] & E) == 0){
					possib[nPossib].x = i-2; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i - 1] & ON) && (grid[j][i + 1] & ON)){	// o x o
				if ((grid[j][i - 1] & E) == 0 && (grid[j][i + 1] & E) == 0){
					possib[nPossib].x = i - 1; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i + 1] & ON) && (grid[j][i + 2] & ON)){	// x o o
				if ((grid[j][i + 1] & E) == 0 && (grid[j][i + 2] & E) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			
			// SE 1
			if ((grid[j-2][i - 2] & ON) && (grid[j-1][i - 1] & ON)){	// o o x
				if ((grid[j-2][i - 2] & SE) == 0 && (grid[j-1][i - 1] & SE) == 0){
					possib[nPossib].x = i - 2; possib[nPossib].y = j-2; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j-1][i - 1] & ON) && (grid[j+1][i + 1] & ON)){	// o x o
				if ((grid[j-1][i - 1] & SE) == 0 && (grid[j+1][i + 1] & SE) == 0){
					possib[nPossib].x = i - 1; possib[nPossib].y = j-1; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j+1][i + 1] & ON) && (grid[j+2][i + 2] & ON)){	// x o o
				if ((grid[j+1][i + 1] & SE) == 0 && (grid[j+2][i + 2] & SE) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 1; nPossib++;
				}
			}
			// S 2
			if ((grid[j - 2][i] & ON) && (grid[j - 1][i] & ON)){	// o o x
				if ((grid[j - 2][i] & S) == 0 && (grid[j - 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 2; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j - 1][i] & ON) && (grid[j + 1][i] & ON)){	// o x o
				if ((grid[j - 1][i] & S) == 0 && (grid[j + 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 1; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j + 1][i] & ON) && (grid[j + 2][i] & ON)){	// x o o
				if ((grid[j + 1][i] & S) == 0 && (grid[j + 2][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 2; nPossib++;
				}
			}
			// SW
			if ((grid[j - 2][i+2] & ON) && (grid[j - 1][i+1] & ON)){	// o o x
				if ((grid[j - 2][i+2] & SW) == 0 && (grid[j - 1][i+1] & SW) == 0){
					possib[nPossib].x = i + 2; possib[nPossib].y = j-2; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j - 1][i+1] & ON) && (grid[j + 1][i-1] & ON)){	// o x o
				if ((grid[j - 1][i+1] & SW) == 0 && (grid[j + 1][i-1] & SW) == 0){
					possib[nPossib].x = i + 1; possib[nPossib].y = j-1; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j + 1][i-1] & ON) && (grid[j + 2][i-2] & ON)){	// x o o
				if ((grid[j + 1][i-1] & SW) == 0 && (grid[j + 2][i-2] & SW) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 3; nPossib++;
				}
			}
			
		}
	}
	if (false){
		printf("level=%d nPossib=%d   %c", level, nPossib, 13);
		for (int i = 0; i < nPossib; i++) printf("[%d,%d,%d]", possib[i].x - W0, possib[i].y - H0, possib[i].d);
		printf("\n");
		exit(1);
	}

	if (nPossib == 0){
		if (level<levelMin){
			levelMin = level;
			fprintf(stderr, "Wow new min %d ! \n", levelMin);
			for (int i = 0; i<levelMin; i++){
				printf("[%d,%d,%s]", coup[i].x - W0, coup[i].y - H0, td[coup[i].d]);
			}
			printf("\n");
			FILE *f = fopen(fout, "a");
			fprintf(f, "Wow new min %d ! \n", levelMin);
			for (int i = 0; i<levelMin; i++){
				fprintf(f,"[%d,%d,%s]", coup[i].x - W0, coup[i].y - H0, td[coup[i].d]);
			}
			fprintf(f,"\n");
			fclose(f);
			dump();
		}
		return;
	}
	PT buf[5];
	int c;
	if (nPossib == 1) c = 0; else if (nPossib == 2) c = rand() & 1; else c = rand() % nPossib;
	trace(possib[c].x, possib[c].y, possib[c].d, buf);
	coup[level].x = possib[c].x; coup[level].y = possib[c].y; coup[level].d = possib[c].d;
	playRand(level + 1);
	untrace(possib[c].x, possib[c].y, possib[c].d, buf);
}
void parse(int level){
	if (level > levelMin) return;
	config++;
	COUP possib[256];
	int nPossib = 0;
	// middle to top
	for (char j = H0; j>=2; j--){
		int nbPoints = 0;
		for (char i = 2; i<WIDTH - 2; i++){
			if ((grid[j][i]) != 0){
				nbPoints++; continue;
			}
			if ((!grid[j - 1][i - 1])  && (!grid[j - 1][i])  && (!grid[j - 1][i + 1]) 
				&& (!grid[j][i - 1])  && (!grid[j][i + 1]) 
				&& (!grid[j + 1][i - 1])  && (!grid[j + 1][i])  && (!grid[j + 1][i + 1]) 
				) continue;	// pas de 8-voisins on arrete 
			// E 0
			if ((grid[j][i - 2] ) && (grid[j][i - 1] )){	// o o x
				if ((grid[j][i - 2] & E) == 0 && (grid[j][i - 1] & E) == 0){
					possib[nPossib].x = i - 2; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i - 1] ) && (grid[j][i + 1] )){	// o x o
				if ((grid[j][i - 1] & E) == 0 && (grid[j][i + 1] & E) == 0/* && (grid[j][i +2] & E) == 0*/){
					possib[nPossib].x = i - 1; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i + 1] ) && (grid[j][i + 2] )){	// x o o
				if ((grid[j][i + 1] & E) == 0 && (grid[j][i + 2] & E) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}

			// SE 1
			if ((grid[j - 2][i - 2] ) && (grid[j - 1][i - 1] )){	// o o x
				if ((grid[j - 2][i - 2] & SE) == 0 && (grid[j - 1][i - 1] & SE) == 0){
					possib[nPossib].x = i - 2; possib[nPossib].y = j - 2; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j - 1][i - 1] ) && (grid[j + 1][i + 1] )){	// o x o
				if ((grid[j - 1][i - 1] & SE) == 0 && (grid[j + 1][i + 1] & SE) == 0){
					possib[nPossib].x = i - 1; possib[nPossib].y = j - 1; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j + 1][i + 1] ) && (grid[j + 2][i + 2] )){	// x o o
				if ((grid[j + 1][i + 1] & SE) == 0 && (grid[j + 2][i + 2] & SE) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 1; nPossib++;
				}
			}
			// S 2
			if ((grid[j - 2][i] ) && (grid[j - 1][i] )){	// o o x
				if ((grid[j - 2][i] & S) == 0 && (grid[j - 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 2; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j - 1][i] ) && (grid[j + 1][i] )){	// o x o
				if ((grid[j - 1][i] & S) == 0 && (grid[j + 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 1; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j + 1][i] ) && (grid[j + 2][i] )){	// x o o
				if ((grid[j + 1][i] & S) == 0 && (grid[j + 2][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 2; nPossib++;
				}
			}
			// SW
			if ((grid[j - 2][i + 2] ) && (grid[j - 1][i + 1] )){	// o o x
				if ((grid[j - 2][i + 2] & SW) == 0 && (grid[j - 1][i + 1] & SW) == 0){
					possib[nPossib].x = i + 2; possib[nPossib].y = j - 2; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j - 1][i + 1] ) && (grid[j + 1][i - 1] )){	// o x o
				if ((grid[j - 1][i + 1] & SW) == 0 && (grid[j + 1][i - 1] & SW) == 0){
					possib[nPossib].x = i + 1; possib[nPossib].y = j - 1; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j + 1][i - 1] ) && (grid[j + 2][i - 2] )){	// x o o
				if ((grid[j + 1][i - 1] & SW) == 0 && (grid[j + 2][i - 2] & SW) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 3; nPossib++;
				}
			}
		}
		if (nbPoints == 0) break;	// full line with no point
	}
	for (char j = H0+1; j<HEIGHT - 2; j++){	// from middle to bottom
		int nbPoints = 0;
		for (char i = 2; i<WIDTH - 2; i++){
			if ((grid[j][i] ) != 0){
				nbPoints++; continue;
			}
			if ((grid[j - 1][i - 1] ) == 0 && (grid[j - 1][i] ) == 0 && (grid[j - 1][i + 1] ) == 0
				&& (grid[j][i - 1] ) == 0 && (grid[j][i + 1] ) == 0
				&& (grid[j + 1][i - 1] ) == 0 && (grid[j + 1][i] ) == 0 && (grid[j + 1][i + 1] ) == 0
				) continue;	// pas de 8-voisins on arrete 
			// E 0
			if ((grid[j][i - 2] ) && (grid[j][i - 1] )){	// o o x
				if ((grid[j][i - 2] & E) == 0 && (grid[j][i - 1] & E) == 0){
					possib[nPossib].x = i - 2; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i - 1] ) && (grid[j][i + 1] )){	// o x o
				if ((grid[j][i - 1] & E) == 0 && (grid[j][i + 1] & E) == 0/* && (grid[j][i +2] & E) == 0*/){
					possib[nPossib].x = i - 1; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}
			if ((grid[j][i + 1] ) && (grid[j][i + 2] )){	// x o o
				if ((grid[j][i + 1] & E) == 0 && (grid[j][i + 2] & E) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 0; nPossib++;
				}
			}

			// SE 1
			if ((grid[j - 2][i - 2] ) && (grid[j - 1][i - 1] )){	// o o x
				if ((grid[j - 2][i - 2] & SE) == 0 && (grid[j - 1][i - 1] & SE) == 0){
					possib[nPossib].x = i - 2; possib[nPossib].y = j - 2; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j - 1][i - 1] ) && (grid[j + 1][i + 1] )){	// o x o
				if ((grid[j - 1][i - 1] & SE) == 0 && (grid[j + 1][i + 1] & SE) == 0){
					possib[nPossib].x = i - 1; possib[nPossib].y = j - 1; possib[nPossib].d = 1; nPossib++;
				}
			}
			if ((grid[j + 1][i + 1] ) && (grid[j + 2][i + 2] )){	// x o o
				if ((grid[j + 1][i + 1] & SE) == 0 && (grid[j + 2][i + 2] & SE) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 1; nPossib++;
				}
			}
			// S 2
			if ((grid[j - 2][i] ) && (grid[j - 1][i] )){	// o o x
				if ((grid[j - 2][i] & S) == 0 && (grid[j - 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 2; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j - 1][i] ) && (grid[j + 1][i] )){	// o x o
				if ((grid[j - 1][i] & S) == 0 && (grid[j + 1][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j - 1; possib[nPossib].d = 2; nPossib++;
				}
			}
			if ((grid[j + 1][i] ) && (grid[j + 2][i] )){	// x o o
				if ((grid[j + 1][i] & S) == 0 && (grid[j + 2][i] & S) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 2; nPossib++;
				}
			}
			// SW
			if ((grid[j - 2][i + 2] ) && (grid[j - 1][i + 1] )){	// o o x
				if ((grid[j - 2][i + 2] & SW) == 0 && (grid[j - 1][i + 1] & SW) == 0){
					possib[nPossib].x = i + 2; possib[nPossib].y = j - 2; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j - 1][i + 1] ) && (grid[j + 1][i - 1] )){	// o x o
				if ((grid[j - 1][i + 1] & SW) == 0 && (grid[j + 1][i - 1] & SW) == 0){
					possib[nPossib].x = i + 1; possib[nPossib].y = j - 1; possib[nPossib].d = 3; nPossib++;
				}
			}
			if ((grid[j + 1][i - 1] ) && (grid[j + 2][i - 2] )){	// x o o
				if ((grid[j + 1][i - 1] & SW) == 0 && (grid[j + 2][i - 2] & SW) == 0){
					possib[nPossib].x = i; possib[nPossib].y = j; possib[nPossib].d = 3; nPossib++;
				}
			}
		}
		if (nbPoints == 0) break;	// full line with no point
	}
	if (false){
		printf("level=%d nPossib=%d   %c", level, nPossib, 13);
		for (int i = 0; i < nPossib; i++) printf("[%d,%d,%d]", possib[i].x - W0, possib[i].y - H0, possib[i].d);
		printf("\n");
		for (int i = 0; i<levelMin; i++){
			printf("[%d,%d,%s]", coup[i].x - W0, coup[i].y - H0, td[coup[i].d]);
		}
		printf("\n");
		exit(1);
	}

	if (nPossib == 0){
		if (level<levelMin){
			levelMin = level;
			fprintf(stderr, "Wow new min %d ! \n", levelMin);
			for (int i = 0; i<levelMin; i++){
				printf("[%d,%d,%s]", coup[i].x - W0, coup[i].y - H0, td[coup[i].d]);
			}
			printf("\n");
			FILE *f = fopen(fout, "a");
			fprintf(f, "Wow new min %d ! \n", levelMin);
			for (int i = 0; i<levelMin; i++){
				fprintf(f, "[%d,%d,%s]", coup[i].x - W0, coup[i].y - H0, td[coup[i].d]);
			}
			fprintf(f,"\n");
			fclose(f);
			dump();
		}
		return;
	}
	PT buf[5];
	for (int c = 0; c < nPossib; c++){
		coup[level].x = possib[c].x; coup[level].y = possib[c].y; coup[level].d = possib[c].d;
		trace(possib[c].x, possib[c].y, possib[c].d, buf);
		parse(level + 1);
		untrace(possib[c].x, possib[c].y, possib[c].d, buf);
	}
}
#define BRUTE 1
#define RAND 2
const int dbg = 0;

void tracetry(char i, char j, char d){
	int can = canTrace(i, j, d);
	if (can < 0){
		if (d == 0){
			can = canTrace(i + 1, j, d); if (can < 0) can = canTrace(i + 2, j, d);
		}
		else if (d == 1){
			can = canTrace(i + 1, j + 1, d); if (can < 0) can = canTrace(i + 2, j + 2, d);
		}
		else if (d == 2){
			can = canTrace(i, j + 1, d); if (can < 0) can = canTrace(i, j + 2, d);
		}
		else if (d == 3){
			can = canTrace(i - 1, j + 1, d); if (can < 0) can = canTrace(i - 2, j + 2, d);
		}
	}
	if (can<0){
		printf("cant trace [%d,%d,%s] (%d,%d,%d) Err %d\n", i - W0, j - H0, td[d], i, j, d, can);
		dump();
		exit(1);
	}
	PT buf[5];
	printf("play [%d,%d,%s] (can=%d)\n", i - W0, j - H0, td[d], can);
	trace(i, j, d, buf);
}
int main(int argc, char* argv[])
{
	int nbDepMax = 999;
	int mode = RAND;
	int i = 1;
	while(i<argc){
		if (!strcmp(argv[i], "-brute")) mode = BRUTE;
		else if (!strcmp(argv[i], "-random")) mode = RAND;
		else if (!strcmp(argv[i], "-dep")){
			i++; sscanf(argv[i], "%d", &nbDepMax);
		}
		else if (argv[i][0] == '-'){
			fprintf(stderr, "option %s non reconnue : use -brute ou -random ou -dep [xxx]\n", argv[1]);
			exit(1);
		}
		else sscanf(argv[i], "%d", &levelMin);
		i++;
	}
	printf("Cherche les grilles 3D avec un score < %d...\n", levelMin);
	printf("ini...\n");
	srand(time(NULL));
	ini();

	strcpy(td[0], "E");
	strcpy(td[1], "SE");
	strcpy(td[2], "S");
	strcpy(td[3], "SW");

	dump(false);
	printf("mode=%d\n", mode);
	if (mode == BRUTE){
		sprintf(fout, "cdm3D-brute-%d.txt", nbDepMax);
		// printf("parse(0)...\n");
		// parse(0);
		if (true){
			PT buf[5];
			int c=0, i, j, d;
			printf("Start. c=%d/%d nbConfig=%lld\n", c, nbDepMax, config, ((double)t2 - (double)t1) / CLOCKS_PER_SEC);
			if (c<nbDepMax){ i = -2, j = 0, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){i = 0, j = 0, d = 2;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++;}
			if (c<nbDepMax){i = 2, j = -1, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++;}
			if (c<nbDepMax){i = -1, j = -2, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++;}
			if (c<nbDepMax){ i = -1, j = 0, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = 0, j = -1, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = 0, j = 2, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = 1, j = 0, d = 2;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = 1, j = 1, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			// 10}
			if (c<nbDepMax){ i = 1, j = -1, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -1, j = 0, d = 2;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = 1, j = 1, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -2, j = 1, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -2, j = -1, d = 2;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
				/* pas mieux que 23 avec 14 premiers coups joues */
			if (c<nbDepMax){ i = -3, j = -1, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -1, j = -2, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -1, j = 0, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -3, j = 0, d = 2;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -4, j = -1, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			// 20}
			if (c<nbDepMax){ i = -5, j = -1, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -3, j = 1, d = 1;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -4, j = 2, d = 0;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			if (c<nbDepMax){ i = -3, j = 1, d = 3;	coup[c].x = W0 + i; coup[c].y = H0 + j; coup[c].d = d; tracetry(coup[c].x, coup[c].y, coup[c].d); c++; }
			dump();

			printf("Start. c=%d nbConfig=%lld\n", c, config, ((double)t2 - (double)t1) / CLOCKS_PER_SEC);
			t1 = clock();
			parse(c);
			t2 = clock();
			printf("Finito. nbConfig=%lld cpu:%lf\n", config, ((double)t2 - (double)t1) / CLOCKS_PER_SEC);
			// untrace(W0 - 2, H0, 0, buf);
		}
	}
	else{
		sprintf(fout, "cdm3D-random-%d.txt", nbDepMax);
		printf("playRand(0)...\n");
		/*
		Wow new min 23 ! nDep=0
		[0,0,E][0,0,S][-1,-2,SE][0,-1,SW][-1,0,SE][1,0,S][-1,-1,SE][1,1,SW][2,-1,SW][0,1,E][0,2,E][-1,-1,E][1,-1,SE][2,-2,SW][2,1,SW][3,1,SW][-2,1,SE][0,3,E][-1,2,S][2,2,S][-1,-3,S][1,3,SE][2,-3,S]
		················
		················
		················
		················
		······+··+······
		······+··+······
		······+O++······
		······OOO+······
		·····+·O+++·····
		······++++······
		······++++······
		······+··+······
		··········+·····
		················
		················
		················


		Wow new min 23 !
		[-2,0,E][0,0,S][2,-1,SW][-1,-2,SE][-1,0,SE][0,-1,E][0,2,E][1,0,S][1,1,SE][1,-1,SW][-1,0,S][1,1,SW][-2,1,E][-2,-1,S][-1,-2,SW][-4,-1,SE][-3,-1,SE][-5,-1,E][-3,-1,S][-2,0,SW][-1,0,SW][-5,2,E][-3,1,SE]
		levelMin=23
		················
		················
		················
		················
		················
		·······+········
		···++++·O++·····
		·····++OOO······
		·····+++O+······
		···++++++++·····
		·······+···+····
		················
		················
		················
		*/
		playRand(0);
		dump();

		for (int i = 0; i<HEIGHT; i++){
			for (int j = 0; j<WIDTH; j++){
				if (gridDepart[i][j] != grid[i][j]) printf("Bug at %d,%d!\n", j, i);
			}
		}

		for (int i = 0; i<100000000; i++){
			if (i % 10000 == 0) printf("i=%d   %c", i, 13);
			// ini();
			playRand(0);
		}
	}
	printf("Finished !\n");
	dump();

	exit(0);

	/*
	t1 = clock();
	parse(0);
	t2 = clock();
	printf("profMax=%d nbConfig=%lld, configMax=%lld, cpu:%lf\n", profMax, config, configMax, ((double)t2 - (double)t1) / CLOCKS_PER_SEC);
	*/
	return 0;
}

