﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INT (((unsigned)(-1)) >> 1)
#define MIN_INT (MAX_INT + 1)

#define INCECO 10
#define PL     0
#define TS     3

typedef enum {
	Nil = 0, Black = 1, White = -1
} Oval;

typedef struct {
	int weight;
	Oval val;
} Grid;

typedef struct {
	Grid grids[TS][TS];
	int score;
	int ngrid;
} Board;

typedef struct {
	int x;
	int y;
} Point;

typedef struct GameTree {
	struct GameTree *prev;
	struct GameTree **branch;
	int _nbranch;
	long _sleaf;
	int depth;
	int nleaf;
	void *leaf;
} GTree;

int    down(Board *, GTree *, const Oval, const int, const int);
int    evaluate(Board *);

Board *bd_cpy(const Board *);
Board *bd_cre(const int [TS][TS]);

void   gt_prt(GTree *const, const int);
GTree *gt_cre(const void *, const long);
GTree *gt_add(GTree *const, const void *, const long);
int    gt_del(GTree *);

unsigned long long m = 0, f = 0, n = 0, k = 0;

int main() {

	time_t start = time(NULL);
	setbuf(stdout, NULL);
	//////////////////////////////
	int u[3][3] = {
		{0, 0, 0},
	    {0, 1, 0},
	    {0, -1, 0}
	};
	Board *bd = bd_cre(u);
	GTree *gt = gt_cre(&bd, sizeof(bd));
	int rs = down(bd, gt, Black, 2, 0);

	gt_prt(gt, 0);
	//printf("%d\n", rs);

	//////////////////////////////
	printf("\nmalloc:%lld, free:%llu\n", m, f);
	printf("\nknots:%lld, iterations:%llu\n", k, n);
	printf("\ntime use:%lds\n\n", (long)(time(NULL) - start));

	system("pause");

	return 0;
}

int down(Board *vbd, GTree *vgt, const Oval nextVal, const int maxdep, const int curdep) {

	if (!maxdep)
		return evaluate(vbd);

	int max = MIN_INT, min = MAX_INT;
	int score = 0;

	for (int i = 0, n = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			if (vbd->grids[i][j].val == Nil) {
				Board *bd = bd_cpy(vbd);
				GTree *gt = gt_add(vgt, &bd, sizeof(bd));

				bd->grids[i][j].val = nextVal;
				bd->score = down(bd, gt, -nextVal, maxdep - 1, curdep + 1);

				max = bd->score > max ? bd->score : max;
				min = bd->score < min ? bd->score : min;
			}
		}
	}

	if (curdep % 2)
		return min;

	return max;
}

int evaluate(Board *vbd) {

	return rand() % 1000;
}

Board *bd_cpy(const Board *vbd) {
	Board *bd = (Board *)memcpy(malloc(sizeof(Board)), vbd, sizeof(Board));
	if (!bd) exit(-233);
	return bd;
}

Board *bd_cre(const int undone[TS][TS]) {

	Board *bd = calloc(1, sizeof(Board));
	if (undone) {
		for (int i = 0; i < TS; ++i) {
			for (int j = 0; j < TS; ++j) {
				bd->grids[i][j].val = (Oval)undone[i][j];
				if (bd->grids[i][j].val == Nil) ++bd->ngrid;
			}
		}
	} else {
		bd->ngrid = TS * TS;
	}
	return bd;
}

void gt_prt(GTree *const gt, const int level) {

	if (PL && level > PL) return;

	const int pfmax = 256;
	char *prefix = (char *)calloc(1, sizeof(char) * pfmax);
	GTree *temp = gt->prev;
	char *se = NULL;

	for (int i = 0; i < level - 1; ++i, temp = temp->prev) {
		if (temp->prev && temp == temp->prev->branch[temp->prev->nleaf - 1])
			se = "      ";
		else
			se = "│     ";
		memmove(prefix + strlen(se), prefix, strlen(prefix) + 1);
		memcpy(prefix, se, strlen(se));
	}
	char *content = (char *)calloc(1, sizeof(char) * 512);
	sprintf(content, "level:%d, score:%d", level, (*((Board **)(gt->leaf)))->score); /////////////

	char **chess = (char **)malloc(sizeof(char *) * TS * TS);
	for (int i = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			switch ((*((Board **)(gt->leaf)))->grids[i][j].val) {
			case  Nil: chess[TS*i + j] = " "; break;
			case  Black: chess[TS*i + j] = "●"; break;
			case  White: chess[TS*i + j] = "○"; break;
			}
		}
	}
	char *p = NULL, *p1 = NULL;
	if (gt->prev) {
		if (gt == gt->prev->branch[gt->prev->nleaf - 1]) {
			p = "└─"; p1 = "  ";
		} else {
			p = "├─"; p1 = "│ ";
		}
		printf("%s%s %s\n", prefix, p, content);
	} else {
		p1 = ""; printf("%s\n", content);
	}

	for (int i = 1, n = 0; i <= 2 * TS + 1; ++i) {
		printf("%s%s", prefix, p1);
		if (i % 2) {
			switch (i) {
			case 1:
				for (int j = 0; j < TS + 1; ++j) {
					switch (j) {
					case 0: printf("┌   "); break;
					case TS: printf("┐ \n"); break;
					default: printf("┬   ");
					}
				}
				break;
			case 2 * TS + 1:
				for (int j = 0; j < TS + 1; ++j) {
					switch (j) {
					case 0: printf("└   "); break;
					case TS: printf("┘ \n"); break;
					default: printf("┴   ");
					}
				}
				break;
			default:
				for (int j = 0; j < TS + 1; ++j) {
					switch (j) {
					case 0: printf("├   "); break;
					case TS: printf("┤ \n"); break;
					default: printf("┼   ");
					}
				}
			}
		} else {
			for (int j = 0; j < TS; ++j)
				printf("  %s ", chess[n++]);
			printf("\n");
		}
	}

	for (int i = 0; i < gt->nleaf; ++i)
		gt_prt(gt->branch[i], level + 1);

	return;
}

GTree *gt_cre(const void *leaf, const long size) {

	if (!leaf) return NULL;

	GTree *gt = (GTree *)malloc(sizeof(GTree)); if (gt) m++;
	if (!gt) return NULL;

	gt->prev = NULL;
	gt->branch = (GTree **)malloc(sizeof(GTree *) * INCECO); if (gt->branch) m++;
	gt->_nbranch = INCECO;
	gt->_sleaf = size;
	gt->depth = gt->nleaf = 0;
	gt->leaf = malloc(size); if (gt->leaf) m++;

	if (!(gt->branch && gt->leaf)) {
		if (gt->branch) f++; free(gt->branch);
		if (gt->leaf) f++; free(gt->leaf);
		if (gt) f++; free(gt);
		return NULL;
	}
	memset(gt->branch, 0, sizeof(GTree *) * INCECO);
	memcpy(gt->leaf, leaf, size); ++k;
	return gt;
}

GTree *gt_add(GTree *const destgt, const void *leaf, const long size) {

	GTree *gt = gt_cre(leaf, size);
	if (!gt) return NULL;

	gt->prev  = destgt;
	gt->depth = gt->prev->depth + 1;
	if (destgt->_nbranch <= destgt->nleaf) {
		GTree **temp = (GTree **)realloc(destgt->branch, sizeof(GTree *) * (destgt->_nbranch + INCECO));
		if (!temp) return NULL;
		memset(temp + destgt->_nbranch, 0, sizeof(GTree *) * INCECO);
		destgt->branch = temp;
		destgt->_nbranch += INCECO;
	}
	return destgt->branch[destgt->nleaf++] = gt;
}

int gt_del(GTree *gt) { // delete all behind gt

	if (gt->prev) {
		for (int i = 0; i < gt->prev->nleaf; ++i) {
			if (gt->prev->branch[i] == gt) {
				gt->prev->branch[i] = NULL;
				if (i + 1 < gt->prev->nleaf)
					memmove(gt->prev->branch + i, gt->prev->branch + i + 1, sizeof(GTree *) * (gt->prev->nleaf - i - 1));
				--gt->prev->nleaf;
				break;
			}
		}
	}
	gt->prev = NULL;
	GTree *alpha = gt;
	GTree *beta = NULL;
	int iterations = 0;

	do {
		if (alpha->nleaf)
			alpha = alpha->branch[alpha->nleaf - 1];
		else {
			if (alpha->branch) f++; free(alpha->branch);
			if (alpha->leaf) f++; free(alpha->leaf);
			beta = alpha->prev;
			if (beta) beta->branch[beta->nleaf-- - 1] = NULL;
			if (alpha) f++; free(alpha);
			alpha = beta;
		}
		iterations++;
	} while (alpha);

	return (int)(n += iterations);
}