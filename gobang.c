#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INCECO 10  // increasing coefficient
#define TS 3      // table size
#define CON 3    // win condition
#define ML 0    // printf max level 0 == all
#define GL TS*TS    // generate level 0 == none, up to TS*TS

typedef struct multitree {

	struct multitree *prev;
	struct multitree **branch;
	int _nbranch;
	int nleaf;
	int rate;
	void *leaf;

} mtree;

int decide(mtree *);
void *getNext(void *, const long, const int);
mtree *generate();
void mt_prt(mtree *const, char *, const int, const int);
mtree *mt_cre(const void *, const long);
mtree *mt_add(mtree *const, const void *, const long);
int mt_del(mtree *);

unsigned long long m = 0, f = 0, n = 0, k = 0;

time_t start;

int main() {

	start = time(NULL);

	mtree *well = generate();

	int i = 0; mtree *mt = well;
	while (mt->nleaf) {
		do {
			for (int j = i = 0; j < mt->nleaf; ++j) {
				i = (mt->branch[j]->rate > mt->branch[i]->rate) ? j : i;
			}
			mt_del(mt->branch[i]);
		} while (mt->nleaf != 1);
		mt = mt->branch[0];
	}
	mt_prt(well, (char *)memset(malloc(sizeof(char)), 0, sizeof(char)), 0, ML);
	mt_del(well);

	printf("\nmalloc:%lld, free:%llu\n", m, f);
	printf("\nknots:%lld, iterations:%llu\n", k, n);
	printf("\ntime use:%lds\n\n", (long)(time(NULL) - start));

	return 0;
}

int decide(mtree *mt) { // 判断结果(1结束，0未完成)，并赋值rate

	int(*p)[TS] = (int(*)[TS])mt->leaf;

	int i, j;
	for (i = 0; i < TS; ++i) {
		for (j = 0; j < TS; ++j) {
			int sum;
		//row:
			if (j <= TS - CON) {
				for (int k = sum = 0; k < CON; ++k)
					sum += p[i][j + k];
				if (abs(sum) == CON)
					goto end;
			}
		//col:
			if (i <= TS - CON) {
				for (int k = sum = 0; k < CON; ++k)
					sum += p[i + k][j];
				if (abs(sum) == CON)
					goto end;
			}
		//sla:
			if (i >= CON - 1 && j <= TS - CON) {
				for (int k = sum = 0; k < CON; ++k)
					sum += p[i - k][j + k];
				if (abs(sum) == CON)
					goto end;
			}
		//bsla:
			if (i <= TS - CON && j <= TS - CON) {
				for (int k = sum = 0; k < CON; ++k)
					sum += p[i + k][j + k];
				if (abs(sum) == CON)
					goto end;
			}
		}
	}
	return 0;

end:
	for (mtree *k = mt; k != NULL; k = k->prev)
		k->rate += p[i][j];
	return 1;
}

void *getNext(void *vprev, const long size, const int priority) {

	// 判断谁的回合，和为1则白，和为0则黑

	int *prev = (int *)memcpy(malloc(size), vprev, size); // 复制内存
	int sum = 0;
	int side = 0;
	for (int i = 0; i < TS * TS; ++i)
		sum += prev[i];

	if (sum == 1)
		side = -1;
	else if (sum == 0)
		side = 1;
	else
		exit(-233);

	for (int i = 0, n = 0; i < TS * TS; ++i)
		if (((int *)vprev)[i] == 0)
			if (++n == priority) prev[i] = side;

	return prev; // 调用方管理内存
}

mtree *generate() {

	int leaf[TS][TS] = { 0 };
	int nleaf = TS * TS;
	unsigned long long tnleaf = 1;
	const int sleaf = sizeof(leaf);
	mtree *root = mt_cre(leaf, sleaf);

	mtree **temp = (mtree **)malloc(sizeof(mtree *));
	if (temp) m++;
	*temp = root;
	mtree **curr = temp;
	mtree **alpha = NULL;

	while (nleaf && nleaf > TS*TS - GL) {
		alpha = (mtree **)calloc(1, sizeof(mtree *) * tnleaf * nleaf);
		if (alpha) m++;
		else exit(-222);  // TS为4时直接就申请内存失败了……

		for (unsigned long long j = 0, n = 0; j < tnleaf; ++j, ++curr)
			for (int i = 0; i < nleaf; ++i, ++n)
				if (*curr &&
					(!(*curr)->prev ||
					((*curr)->prev && !decide(*curr)))) { // 在decide中赋值rate
					void *next = getNext((*curr)->leaf, sleaf, i + 1);
					alpha[n] = mt_add(*curr, next, sleaf);
					free(next);
				}
		if (temp) f++;
		free(temp);
		curr = temp = alpha;
		tnleaf *= nleaf--;
	}

	return root;
}

void mt_prt(mtree *const mt, char *pf, const int level, const int maxlevel) { // not manage the memory yet
	
	if (maxlevel && level > maxlevel) return;

	const int pfmax = 256;
	char *prefix =
		(char *)(memset(malloc(sizeof(char) * pfmax), 0, sizeof(char) * pfmax));

	mtree *temp = mt->prev;
	char *se = NULL;
	for (int i = 0; i < level - 1; ++i, temp = temp->prev) {
		if (temp->prev && temp == temp->prev->branch[temp->prev->nleaf - 1])
			se = "      ";
		else
			se = "│     ";
		int l = strlen(prefix) + 1;
		memmove(prefix + strlen(se), prefix, l);
		memcpy(prefix, se, strlen(se));
	}
	char *content = (char *)calloc(1, sizeof(char) * 512);
	sprintf(content, "level:%d, rate:%d", level, mt->rate); /////////////

	char **chess = (char **)malloc(sizeof(char *) * TS*TS);
	for (int i = 0; i < TS*TS; ++i)
		switch (((int *)mt->leaf)[i]) {
		case 0:
			chess[i] = " ";
			break;
		case 1:
			chess[i] = "●";
			break;
		case -1:
			chess[i] = "○";
			break;
		}

	char *p = NULL, *p1 = NULL;
	if (mt->prev) {
		if (mt == mt->prev->branch[mt->prev->nleaf - 1]) {
			p = "└─";
			p1 = "  ";
		}
		else {
			p = "├─";
			p1 = "│ ";
		}
		printf("%s%s %s\n", prefix, p, content);
	}
	else {
		p1 = "";
		printf("%s\n", content);
	}

	///////////////////////////////
	int n = 0;
	for (int i = 1; i <= 2*TS + 1; ++i) {
		printf("%s%s", prefix, p1);
		if (i % 2) {
			switch (i) {
				case 1:
					for (int j = 0; j < TS + 1; ++j) {
						switch (j) {
							case 0:
								printf("┌   ");
								break;
							case TS:
								printf("┐\n");
								break;
							default:
								printf("┬   ");
						}
					}
					break;
				case 2*TS + 1:
					for (int j = 0; j < TS + 1; ++j) {
						switch (j) {
							case 0:
								printf("└   ");
								break;
							case TS:
								printf("┘\n");
								break;
							default:
								printf("┴   ");
						}
					}
					break;
				default:
					for (int j = 0; j < TS + 1; ++j) {
						switch (j) {
							case 0:
								printf("├   ");
								break;
							case TS:
								printf("┤\n");
								break;
							default:
								printf("┼   ");
						}
					}
			}
		} else {
			for (int j = 0; j < TS; ++j)
				printf("  %s ", chess[n++]);
			printf("\n");
		}
	}
	// printf("%s%s┌   ┬   ┬   ┐\n", prefix, p1);
	// printf("%s%s  %s   %s   %s \n", prefix, p1, chess[0], chess[1], chess[2]);
	// printf("%s%s├   ┼   ┼   ┤\n", prefix, p1);
	// printf("%s%s  %s   %s   %s \n", prefix, p1, chess[3], chess[4], chess[5]);
	// printf("%s%s├   ┼   ┼   ┤\n", prefix, p1);
	// printf("%s%s  %s   %s   %s \n", prefix, p1, chess[6], chess[7], chess[8]);
	// printf("%s%s└   ┴   ┴   ┘\n", prefix, p1);

	for (int i = 0; i < mt->nleaf; ++i)
		mt_prt(mt->branch[i], NULL, level + 1, maxlevel);

	return;
}

mtree *mt_cre(const void *leaf, const long size) {

	if (!leaf) return NULL;

	mtree *mt = (mtree *)malloc(sizeof(mtree));
	if (mt) m++;
	if (!mt) return NULL;

	mt->prev = NULL;
	mt->branch = (mtree **)malloc(sizeof(mtree *) * INCECO);
	if (mt->branch) m++;
	mt->_nbranch = INCECO;
	mt->nleaf = 0;
	mt->rate = 0;
	mt->leaf = malloc(size);
	if (mt->leaf) m++;
	if (!(mt->branch && mt->leaf)) {
		if (mt->branch) f++;
		free(mt->branch);
		if (mt->leaf) f++;
		free(mt->leaf);
		if (mt) f++;
		free(mt);
		return NULL;
	}
	memset(mt->branch, 0, sizeof(mtree *) * INCECO);
	memcpy(mt->leaf, leaf, size);
	++k;
	return mt;
}

mtree *mt_add(mtree *const destmt, const void *leaf, const long size) {

	mtree *mt = mt_cre(leaf, size);
	if (!mt) return NULL;

	mt->prev = destmt;
	if (destmt->_nbranch <= destmt->nleaf) {
		mtree **temp = (mtree **)realloc(
			destmt->branch, sizeof(mtree *) * (destmt->_nbranch + INCECO));
		if (!temp) return NULL;
		memset(temp + destmt->_nbranch, 0, sizeof(mtree *) * INCECO);
		destmt->branch = temp;
		destmt->_nbranch += INCECO;
	}
	return destmt->branch[destmt->nleaf++] = mt;
}

int mt_del(mtree *mt) { // delete all behind mt

	//printf("\ndel:%p\n\n", mt);

	if (mt->prev)
		for (int i = 0; i < mt->prev->nleaf; ++i)
			if (mt->prev->branch[i] == mt) {
				mt->prev->branch[i] = NULL;
				if (i + 1 < mt->prev->nleaf) {
					//printf("mov:dest %p, src %p, size:%d\n\n", mt->prev->branch + i, mt->prev->branch + i + 1, sizeof(mtree *) * (mt->prev->nleaf - i - 1));
					memmove(mt->prev->branch + i, mt->prev->branch + i + 1,
						sizeof(mtree *) * (mt->prev->nleaf - i - 1));
				}
				mt->prev->nleaf--;
				break;
			}
	mt->prev = NULL;
	mtree *alpha = mt;
	mtree *beta = NULL;
	int iterations = 0;

	do {
		if (alpha->nleaf)
			alpha = alpha->branch[alpha->nleaf - 1];
		else {
			if (alpha->branch) f++;
			free(alpha->branch);
			if (alpha->leaf) f++;
			free(alpha->leaf);
			beta = alpha->prev;
			if (beta) beta->branch[beta->nleaf-- - 1] = NULL;
			if (alpha) f++;
			free(alpha);
			alpha = beta;
		}
		iterations++;
	} while (alpha);
	return n = iterations;
}