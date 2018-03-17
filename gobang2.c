#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INT (((unsigned)(-1)) >> 1)
#define MIN_INT (MAX_INT + 1)

#define INCECO 10

#define TS     10
#define CON    5

#define DEP    3
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
	Oval val;
	Oval who;
	int  score;
} Rule;

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
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //  1~10
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // 11~20
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // 21~30
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // 31~40
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },  // 41~50
	    { 0, 0, 0, 0, 0, 0, 0, 0, 1,-1 },  // 51~60
	    { 0, 0, 0, 0, 0, 0, 0, 1, 0,-1 },  // 61~70
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0,-1 },  // 71~80
	    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // 81~90
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  // 91~100
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
						if (!GT && *rbd != vbd) free(*rbd);  // xdpb
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

	// 对一个state使用solve/down后会计算此state下一层
	// curdep实际代表以0开始的、当前state层
	// 但当前state层的max与minscore是下一层的

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

	char state[256] = { 0 };
	int  score = 0;
	char who = 'N';
	Oval val = Nil;
	Rule rule = {233, Nil, 0};
	
	GTree *curgt = NULL, *root = gt_cre(&rule, sizeof(Rule));  // #xooo#

	while (fscanf(file, "%s %d %c", state, &score, &who) != EOF) {
		//printf("[%s]:[%d] %c\n", state, score, who);
		curgt = root;
		for (int i = 0, len = strlen(state); i < len; ++i) {
			switch (state[i]) {
			case '#': val = Nil;   break;
			case 'x': val = Black; break;
			case 'o': val = White; break;
			}
			for (int j = 0, nleaf = curgt->nleaf; j < nleaf; ++j) {
				if (((Rule *)curgt->branch[j]->leaf)->val == val) {
					curgt = curgt->branch[j];
					goto jump;
				}
			}
			curgt = gt_add(curgt, &rule, sizeof(Rule));
			((Rule *)curgt->leaf)->val = val;
		jump:
			if (i == len - 1) {  // the last one
				((Rule *)curgt->leaf)->score = score;
				((Rule *)curgt->leaf)->who = (who == 'B') ? Black : 
					                        ((who == 'W') ? White : Nil);
			}
		}
	}
	fclose(file);

	return root;
}

int evaluate(Board *vbd) {

	// Oval aiVal; Rule rules;
	int blackScore = 0;
	int whiteScore = 0;
	GTree *curgt = rules;
	const Grid(*gbd)[TS] = vbd->grids;
	
	// 从左到右
	for (int i = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[i][j].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (j == TS - 1) goto jump1_1;  // 到达一行最后一项直接结算分数
					else goto jump1_2;
				}
			}
			if (j > 0) --j;  // 未匹配到节点，回退一项
		jump1_1:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules; 
		jump1_2:
			continue;
		}
	}
	// 从上到下
	for (int i = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[j][i].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (j == TS - 1) goto jump2_1;  // 到达一行最后一项直接结算分数
					else goto jump2_2;
				}
			}
			if (j > 0) --j;  // 未匹配到节点，回退一项
		jump2_1:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules;
		jump2_2:
			continue;
		}
	}
	// sla: 左下->右上
	for (int i = 0; i < TS; ++i) {  // first stage
		for (int j = 0; j < i + 1; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[i - j][j].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (i - j == 0) goto jump3_1;  // 到达一行最后一项直接结算分数
					else goto jump3_2;
				}
			}
			if (j > 0) --j;  // 未匹配到节点，回退一项
		jump3_1:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules;
		jump3_2:
			continue;
		}
	}
	for (int i = 0; i < TS - 1; ++i) {  // second stage
		for (int j = 1; j < TS - i; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[TS - j][i + j].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (i + j == TS - 1) goto jump3_3;  // 到达一行最后一项直接结算分数
					else goto jump3_4;
				}
			}
			if (j > 1) --j;  // 未匹配到节点，回退一项
		jump3_3:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules;
		jump3_4:
			continue;
		}
	}
	// bsla: 左上->右下
	for (int i = 0; i < TS; ++i) {  // first stage
		for (int j = 0; j < i + 1; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[j][TS - 1 - i + j].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (j - i == 0) goto jump4_1;  // 到达一行最后一项直接结算分数
					else goto jump4_2;
				}
			}
			if (j > 0) --j;  // 未匹配到节点，回退一项
		jump4_1:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules;
		jump4_2:
			continue;
		}
	}
	for (int i = 1; i < TS; ++i) {  // second stage
		for (int j = 0; j < TS - i; ++j) {
			for (int k = 0, n = curgt->nleaf; k < n; ++k) {
				if (gbd[i + j][j].val == ((Rule *)curgt->branch[k]->leaf)->val) {
					curgt = curgt->branch[k];
					if (i + j == TS - 1) goto jump4_3;  // 到达一行最后一项直接结算分数
					else goto jump4_4;
				}
			}
			if (j > 0) --j;  // 未匹配到节点，回退一项
		jump4_3:
			switch (((Rule *)curgt->leaf)->who) {
			case Black: blackScore += ((Rule *)curgt->leaf)->score; break;
			case White: whiteScore += ((Rule *)curgt->leaf)->score; break;
			}
			curgt = rules;
		jump4_4:
			continue;
		}
	}
	///////////////////////
	switch (aiVal) {
	case Black: return blackScore - whiteScore;
	case White: return whiteScore - blackScore;
	default:    return -233;
	}
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
	//sprintf(content, "[%c]:[%d] %s", (((Rule *)gt->leaf)->val == Nil ? '#' : (((Rule *)gt->leaf)->val == Black ? 'x' : (((Rule *)gt->leaf)->val == White ? 'o' : '*'))), ((Rule *)gt->leaf)->score, (((Rule *)gt->leaf)->who == Nil ? "N" : (((Rule *)gt->leaf)->who == Black ? "B" : (((Rule *)gt->leaf)->who == White ? "W" : "*"))));

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

	//////////////////////
	char **chess = (char **)malloc(sizeof(char *) * TS * TS);
	for (int i = 0; i < TS; ++i) {
		for (int j = 0; j < TS; ++j) {
			switch ((*((Board **)(gt->leaf)))->grids[i][j].val) {
			case  Nil: chess[TS * i + j] = " "; break;
			case  Black: chess[TS * i + j] = "●"; break;
			case  White: chess[TS * i + j] = "○"; break;
			}
		}
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
	free(chess);
	/////////////////////*/
	free(prefix);
	free(content);

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