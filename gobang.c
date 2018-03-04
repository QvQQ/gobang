#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INCECO 10 // increasing coefficient
#define TS 15     // table size
#define CON 5     // win condition
#define ML 0      // printf max level 0 == all
#define DEP 2     // search depth 0 == all

#define true 1
#define false 0

typedef struct gametree {

	struct gametree *prev;
	struct gametree **branch;
	int _nbranch;
	long _sleaf;
	int nleaf;

	int score;

	void *leaf;
} gtree;

void *solve(gtree *, int);
int evaluate(gtree *);
int decide(gtree *);
void *getNext(void *, const int, const long, const int);
//gtree *generate();
void gt_prt(gtree *const, const int);
gtree *gt_cre(const void *, const long);
gtree *gt_add(gtree *const, const void *, const long);
int gt_del(gtree *);

int getScore(int (*)[TS], int);

unsigned long long m = 0, f = 0, n = 0, k = 0, de = 0, eval = 0;

time_t start;

int main() {

	start = time(NULL);

	//setbuf(stdout, NULL);

	int p[][TS] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, -1, -1, -1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
	};

	gtree *input = gt_cre((void *)p, sizeof(p));
	void *solution = solve(input, DEP);
	gtree *output = gt_cre(solution, sizeof(int) * TS * TS);

	//gt_prt(input, 0);
	gt_prt(output, 0);

	gt_del(input);
	gt_del(output);

	printf("\nmalloc:%lld, free:%llu\n", m, f);
	printf("\nknots:%lld, iterations:%llu\n", k, n);
	printf("\ndecide:%lld, evaluate:%lld\n", de, eval);
	printf("\ntime use:%lds\n\n", (long)(time(NULL) - start));

	//system("pause");

	return 0;
}

int getScore(int (*board)[TS], int aiColor) {
	int order = TS;
	/*请指定棋盘阶数order，例如order=7-1 = 6*/
	int count = 0;
	//用来计数横，竖，斜的连续棋子个数
	int socore[] = { 0, 1, 10, 1000, 10000 };
	//如果不是ai方的棋子，那么count归零，且为vlaue加上socore[count]的分数
	int value[4] = { 0, 0, 0, 0 };
	//分别用来计数横，竖，斜的分数，取决于count
	int i = 0; int j = 0;

	int win = false;
	int winN = 5;//必胜连子数

	//横计分
	for (i = 0, j = 0; (i < order) && (win == false); i++) {
		value[0] += socore[count];
		for (j = 0, count = 0; (j < order); j++) {
			if (board[i][j] == aiColor)
			{
				count++;

				if (count == winN)//如果连子数已经达到了必胜条件
				{
					win = true;
					break;
				}
			}
			else {
				value[0] += socore[count];
				count = 0;
			}
		}
	}

	//竖计分
	for (i = 0, j = 0; (j < order) && (win == false); j++) {
		value[1] += socore[count];
		for (i = 0, count = 0; (i < order); i++) {
			if (board[i][j] == aiColor)
			{
				count++;

				if (count == winN)
				{
					win = true;
					break;
				}
			}
			else {
				value[1] += socore[count];
				count = 0;
			}
		}
	}

	//左斜计分
	//左斜上计分
	for (i = 0, j = 0; (j < order) && (win == false); j++) {
		value[2] += socore[count];
		int i2 = i;
		int j2 = j;
		//i2保留i
		for (count = 0; (i < order) && (j < order); i++, j++) {
			if (board[i][j] == aiColor)
			{
				count++;

				if (count == winN)
				{
					win = true;
					break;
				}
			}
			else {
				//printf_s("ok  %d  ",count);
				value[2] += socore[count];
				count = 0;
			}
		}
		i = i2;
		j = j2;
	}

	//左斜下计分
	for (i = 1, j = 0; (i < order) && (win == false); i++) {
		value[2] += socore[count];
		int i2 = i;
		int j2 = j;
		//i2保留i
		for (count = 0; (i < order) && (j < order); i++, j++) {
			if (board[i][j] == aiColor)
			{
				count++;

				if (count == winN)
				{
					win = true;
					break;
				}
			}
			else {
				value[2] += socore[count];
				count = 0;

			}
		}
		i = i2;
		j = j2;
	}


	//右斜计分
	//右斜上计分
	for (i = 0, j = 0; (j < order) && (win == false); j++) {
		int i2 = i;
		int j2 = j;
		//i2保留i
		value[3] += socore[count];
		for (count = 0; (i < order) && (j >= 0); i++, j--) {
			if (board[i][j] == aiColor)
			{
				count++;

				if (count == winN)
				{
					win = true;
					break;
				}
			}
			else {
				value[3] += socore[count];
				count = 0;

			}
		}
		i = i2;
		j = j2;
	}
	//右斜下计分
	for (i = order - 1, j = 1; (j < order) && (win == false); j++) {
		int i2 = i;
		int j2 = j;
		//i2保留i
		value[3] += socore[count];
		for (count = 0; (i > 0) && (j < order); i--, j++) {
			if (board[i][j] == aiColor)
			{
				count++;
				if (count == winN)
				{
					win = true;
					break;
				}
			}
			else {
				value[3] += socore[count];
				count = 0;

			}
		}
		i = i2;
		j = j2;
	}
	if (win == true)
	{
		return 10000000;
	}
	else {
		return value[0] + value[1] + value[2] + value[3];
	}
}

void *solve(gtree *root, int depth) {

	if (!root) return NULL;
	if (!depth) depth = TS * TS;
	if (depth > TS * TS || depth < 0) return root->leaf;

	gtree *curgt = root;
	int(*p)[TS] = (int(*)[TS])root->leaf;
	int *re = (int *)calloc(1, sizeof(int) * depth);
	int curdep = 0;

	do {
		// 统计空闲落子个数
		int ngrid = 0;
		int *b = (int *)curgt->leaf;
		for (int i = 0; i < TS * TS; ++i)
			if (!b[i]) ++ngrid;

		// 往下到末节点
		while (curdep < depth && !decide(curgt) && re[curdep] < ngrid) {
			void *next = getNext(curgt->leaf, curdep % 2 ? -1 : 1, curgt->_sleaf, re[curdep] + 1);
			if (!next) exit(-221);
			gt_add(curgt, next, curgt->_sleaf);
			if (next) f++; free(next);
			curgt = curgt->branch[re[curdep++]++];
		}

		if (curdep == depth || decide(curgt)) {
			curgt->score = evaluate(curgt);  // 评估
		} else {
			re[curdep] = 0;                    // 重置
			int val = curgt->branch[0]->score; // 值为首元素！
			for (int i = 0; i < curgt->nleaf; ++i) {
				if ((curdep + 1) % 2) { // 奇数层，选最大
					val = (curgt->branch[i]->score > val)
					      ? curgt->branch[i]->score
					      : val;
				} else { // 偶数层，选最小
					val = (curgt->branch[i]->score < val)
					      ? curgt->branch[i]->score
					      : val;
				}
			}
			curgt->score = val;
		}

		// 回溯到未得分节点
		while (curgt && curgt->score && curgt != root) {
			for (int i = 0; i < curgt->nleaf; ++i) {
				curgt->branch[i]->prev = NULL;
				gt_del(curgt->branch[i]);
				curgt->branch[i] = NULL;
			}
			curgt->nleaf = 0;
			// 以上为删除检索过的节点，释放内存
			curgt = curgt->prev;
			--curdep;
		}
	} while (!root->score);

	int i = 0, (*rs)[TS] = NULL;
	for (; i < root->nleaf; ++i)
		if (root->branch[i]->score == root->score)
			rs = (int(*)[TS])memcpy(malloc(root->_sleaf), root->branch[i]->leaf,
			                        root->branch[i]->_sleaf);
	//gt_del(root);
	return (void *)rs;
}

int evaluate(gtree *gt) { //////评估函数
	++eval;
	int score1 = getScore((int(*)[TS])gt->leaf, 1);
	int score2 = getScore((int(*)[TS])gt->leaf, -1);

	return (score1 - score2) ? score1 - score2 : -1;
}

int decide(gtree *gt) { // 判断结果(1结束，0未完成)，并赋值rate
	++de;
	int(*p)[TS] = (int(*)[TS])gt->leaf;

	int i, j, k, sum, n = -1;
	for (i = 0; i < TS; ++i) {
		for (j = 0; j < TS; ++j) {
			if (!p[i][j] && !(n = p[i][j])) continue;
			// row:
			if (j <= TS - CON) {
				for (k = sum = 0; k < CON; ++k)
					sum += p[i][j + k];
				if (abs(sum) == CON) return 1;
			}
			// col:
			if (i <= TS - CON) {
				for (k = sum = 0; k < CON; ++k)
					sum += p[i + k][j];
				if (abs(sum) == CON) return 1;
			}
			// sla:
			if (i >= CON - 1 && j <= TS - CON) {
				for (k = sum = 0; k < CON; ++k)
					sum += p[i - k][j + k];
				if (abs(sum) == CON) return 1;
			}
			// bsla:
			if (i <= TS - CON && j <= TS - CON) {
				for (k = sum = 0; k < CON; ++k)
					sum += p[i + k][j + k];
				if (abs(sum) == CON) return 1;
			}
		}
	}
	return abs(n);
}

void *getNext(void *vprev, const int side, const long size, const int priority) {

	int *prev = (int *)memcpy(malloc(size), vprev, size);
	if (prev) m++;

	for (int i = 0, n = 0; i < TS * TS; ++i)
		if (((int *)vprev)[i] == 0)
			if (++n == priority) { prev[i] = side; break; }

	return prev;
}

void gt_prt(gtree *const gt, const int level) { // not manage the memory yet

	if (ML && level > ML) return;

	const int pfmax = 256;
	char *prefix = (char *)calloc(1, sizeof(char) * pfmax);
	gtree *temp = gt->prev;
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
	sprintf(content, "level:%d, score:%d", level, gt->score); /////////////

	char **chess = (char **)malloc(sizeof(char *) * TS * TS);
	for (int i = 0; i < TS * TS; ++i) {
		switch (((int *)gt->leaf)[i]) {
		case  0: chess[i] = " "; break;
		case  1: chess[i] = "●"; break;
		case -1: chess[i] = "○"; break;
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

	///////////////////////////////
	int n = 0;
	for (int i = 1; i <= 2 * TS + 1; ++i) {
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

gtree *gt_cre(const void *leaf, const long size) {

	if (!leaf) return NULL;

	gtree *gt = (gtree *)malloc(sizeof(gtree)); if (gt) m++;
	if (!gt) return NULL;

	gt->prev = NULL;
	gt->branch = (gtree **)malloc(sizeof(gtree *) * INCECO); if (gt->branch) m++;
	gt->_nbranch = INCECO;
	gt->nleaf = gt->score = 0;
	gt->_sleaf = size;
	gt->leaf = malloc(size); if (gt->leaf) m++;
	if (!(gt->branch && gt->leaf)) {
		if (gt->branch) f++; free(gt->branch);
		if (gt->leaf) f++; free(gt->leaf);
		if (gt) f++; free(gt);
		return NULL;
	}
	memset(gt->branch, 0, sizeof(gtree *) * INCECO);
	memcpy(gt->leaf, leaf, size); ++k;
	return gt;
}

gtree *gt_add(gtree *const destgt, const void *leaf, const long size) {

	gtree *gt = gt_cre(leaf, size);
	if (!gt) return NULL;

	gt->prev = destgt;
	if (destgt->_nbranch <= destgt->nleaf) {
		gtree **temp = (gtree **)realloc(destgt->branch, sizeof(gtree *) * (destgt->_nbranch + INCECO));
		if (!temp) return NULL;
		memset(temp + destgt->_nbranch, 0, sizeof(gtree *) * INCECO);
		destgt->branch = temp;
		destgt->_nbranch += INCECO;
	}
	return destgt->branch[destgt->nleaf++] = gt;
}

int gt_del(gtree *gt) { // delete all behind gt

	if (gt->prev) {
		for (int i = 0; i < gt->prev->nleaf; ++i) {
			if (gt->prev->branch[i] == gt) {
				gt->prev->branch[i] = NULL;
				if (i + 1 < gt->prev->nleaf)
					memmove(gt->prev->branch + i, gt->prev->branch + i + 1, sizeof(gtree *) * (gt->prev->nleaf - i - 1));
				--gt->prev->nleaf;
				break;
			}
		}
	}
	gt->prev = NULL;
	gtree *alpha = gt;
	gtree *beta = NULL;
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