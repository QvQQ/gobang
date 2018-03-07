#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INT (((unsigned)(-1)) >> 1)
#define MIN_INT (MAX_INT + 1)

#define INCECO 10

#define TS     10
#define CON    5

#define DEP    4
#define RUL    "rules.txt"

#define GT     0
#define PL     0

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

typedef struct {
	const char *state;
	const int score;
} rule;

typedef struct GameTree {
	struct GameTree *prev;
	struct GameTree **branch;
	int _nbranch;
	long _sleaf;
	int depth;
	int nleaf;
	void *leaf;
} GTree;

Point  solve(Board *, GTree *, const int, Board **);
int    down(Board *, GTree *, const Oval, const int, const int);
GTree *init_rules(const char *);
int    evaluate(Board *);
int    isfinish(Board *, const int, const int);

Board *bd_cpy(const Board *);
Board *bd_cre(const int [TS][TS]);

void   gt_prt(GTree *const, const int);
GTree *gt_cre(const void *, const long);
GTree *gt_add(GTree *const, const void *, const long);
int    gt_del(GTree *);

unsigned long long m = 0, f = 0, n = 0, k = 0;
Oval aiVal = Nil; GTree *rules = NULL;

int main() {

	time_t start = time(NULL);
	setbuf(stdout, NULL);

	//////////////////////////////

	int u[][TS] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	Board *bd = NULL; GTree *gt = NULL; Point pos;

	bd = bd_cre(u);
	if (GT) gt = gt_cre(&bd, sizeof(bd));
	pos = solve(bd, gt, DEP, &bd);

	if (GT) {
		gt_prt(gt, 0);
		gt_del(gt);
	}
	printf("\nRESULT:\n\n");
	gt = gt_cre(&bd, sizeof(bd));
	gt_prt(gt, 0);
	gt_del(gt);

	printf("\n(%d, %d)\n", pos.x, pos.y);

	//////////////////////////////
	printf("\nmalloc:%lld, free:%llu\n", m, f);
	printf("\nknots:%lld, iterations:%llu\n", k, n);
	printf("\ntime use:%lds\n\n", (long)(time(NULL) - start));

	system("pause");

	return 0;
}

Point solve(Board *vbd, GTree *vgt, const int maxdep, Board **rbd) {

	Board *bd = NULL;
	GTree *gt = NULL;
	vbd->score = MIN_INT;
	Point pos = { -1, -1 };

	if (maxdep <= 0)
		return pos;

	int sum = 0;
	for (int i = 0; i < TS; ++i)
		for (int j = 0; j < TS; ++j)
			sum += (int)(vbd->grids[i][j].val);

	Oval nextVal = aiVal = sum ? White : Black;
	rules = init_rules(RUL);

	for (int i = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			if (vbd->grids[i][j].val == Nil) {
				bd = bd_cpy(vbd);
				if (GT) gt = gt_add(vgt, &bd, sizeof(bd));

				bd->grids[i][j].val = nextVal;
				bd->score = (maxdep - 1) && !isfinish(bd, i, j)
					? down(bd, gt, -nextVal, maxdep - 1, 1)
					: evaluate(bd); ////

				if (vbd->score < bd->score) {
					pos.x = i + 1;
					pos.y = j + 1;

					if (rbd) {
						if (!GT) free(*rbd);
						*rbd = bd;
					}
					vbd->score = bd->score;
				} else if (!GT) free(bd);
			}
		}
	}
	return pos;
}

int down(Board *vbd, GTree *vgt, const Oval nextVal, const int maxdep, const int curdep) {

	int max = MIN_INT, min = MAX_INT;
	Board *bd = NULL;
	GTree *gt = NULL;
	int score = 0;

	for (int i = 0, n = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			if (vbd->grids[i][j].val == Nil) {
				bd = bd_cpy(vbd);
				if (GT) gt = gt_add(vgt, &bd, sizeof(bd));

				bd->grids[i][j].val = nextVal;
				bd->score = (maxdep - 1 && !isfinish(bd, i, j)) 
					                     ? down(bd, gt, -nextVal, maxdep - 1, curdep + 1)
					                     : evaluate(bd);

				max = bd->score > max ? bd->score : max;
				min = bd->score < min ? bd->score : min;
				if (!GT) free(bd);
			}
		}
	}
	if (curdep % 2)
		return min;
	return max;
}

GTree *init_rules(const char *path) {

	if (!path)
		path = "rules.txt";

	FILE *file = fopen(path, "r");

	if (!file)
		exit(-777);

	char *p = NULL;
	fscanf(file, "%s", p);
	printf("p:\"%s\"\n\n", p);
	exit(0);


	fclose(file);

	return NULL;
}

int evaluate(Board *vbd) {

	// aiVal;

	return rand() % 10000;
}

int isfinish(Board *vbd, const int row, const int col) {

	int sum, k, ox, oy;
	// row:
	ox = row; oy = col;
	while (oy && vbd->grids[ox][oy - 1].val == vbd->grids[ox][oy].val)
		--oy;
	if (oy <= TS - CON) {
		for (k = sum = 0; k < CON; ++k)
			sum += vbd->grids[ox][oy + k].val;
		if (abs(sum) == CON) return 1;
	}
	// col:
	ox = row; oy = col;
	while (ox && vbd->grids[ox - 1][oy].val == vbd->grids[ox][oy].val)
		--ox;
	if (ox <= TS - CON) {
		for (k = sum = 0; k < CON; ++k)
			sum += vbd->grids[ox + k][oy].val;
		if (abs(sum) == CON) return 1;
	}
	// sla: 左下->右上
	ox = row; oy = col;
	while (ox < TS && oy && vbd->grids[ox + 1][oy - 1].val == vbd->grids[ox][oy].val) {
		++ox; --oy;
	}
	if (ox >= CON - 1 && oy <= TS - CON) {
		for (k = sum = 0; k < CON; ++k)
			sum += vbd->grids[ox - k][oy + k].val;
		if (abs(sum) == CON) return 1;
	}
	// bsla: 左上->右下
	ox = row; oy = col;
	while (ox && oy && vbd->grids[ox - 1][oy - 1].val == vbd->grids[ox][oy].val) {
		--ox; --oy;
	}
	if (ox <= TS - CON && oy <= TS - CON) {
		for (k = sum = 0; k < CON; ++k)
			sum += vbd->grids[ox + k][oy + k].val;
		if (abs(sum) == CON) return 1;
	}
	return 0;
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
	free(prefix);
	free(content);
	free(chess);

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