#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INCECO 10 // increasing coefficient
#define TS 3      // table size
#define CON 3     // win condition
#define ML 0      // printf max level 0 == all
#define GL TS *TS // generate level 0 == none, up to TS*TS

typedef struct gametree {

    struct gametree *prev;
    struct gametree **branch;
    int _nbranch;
    long _sleaf;
    int nleaf;
    int score;
    int alpha;
    int beta;
    void *leaf;
} gtree;

void *solve(gtree *, int);
int evaluate(gtree *);
int decide(gtree *);
void *getNext(void *, const long, const int);
gtree *generate();
void gt_prt(gtree *const, const int);
gtree *gt_cre(const void *, const long);
gtree *gt_add(gtree *const, const void *, const long);
int gt_del(gtree *);

unsigned long long m = 0, f = 0, n = 0, k = 0;

time_t start;

int main() {

    start = time(NULL);

    setbuf(stdout, NULL);

    // gtree *well = generate();

    int p[][TS] = {
    	{0, 0, 0}, 
        {0, 0, 0}, 
        {0, 0, 0}
    };

    gtree *input = gt_cre((void *)p, sizeof(p));
    void *solution = solve(input, 0);
    gtree *output = gt_cre(solution, sizeof(int) * TS * TS);

    gt_prt(input, 0);

    gt_del(input);
    gt_del(output);

    printf("\nmalloc:%lld, free:%llu\n", m, f);
    printf("\nknots:%lld, iterations:%llu\n", k, n);
    printf("\ntime use:%lds\n\n", (long)(time(NULL) - start));

    return 0;
}

void *solve(gtree *root, int depth) {

    if (!root) return NULL;
    if (!depth) depth = TS * TS;
    if (depth > TS * TS || depth < 0) return root->leaf;

    gtree *curgt = root;
    int(*p)[TS] = (int(*)[TS])root->leaf;
    int *re = (int *)calloc(1, sizeof(int) * depth);
    int curdep = 0;

    int n = 0;
    do {
        // 统计空闲落子个数
        int ngrid = 0;
        int *b = (int *)curgt->leaf;
        for (int i = 0; i < TS * TS; ++i) {
            if (!b[i]) ++ngrid;
        }

        // 往下到末节点
        //printf("下沿\n\n");
        while (curdep < depth && !decide(curgt) && re[curdep] < ngrid) {
            //printf("curdep:%d\n\nre[%d]:%d\n\n", curdep, curdep, re[curdep]);
            void *next = getNext(curgt->leaf, curgt->_sleaf, re[curdep] + 1);
            if (!next) exit(-221);
            gt_add(curgt, next, curgt->_sleaf);
            if (next) f++;
            free(next);
            curgt = curgt->branch[re[curdep++]++];
        }
        //printf("下沿条件：\ncurdep(%d) < depth(%d)\n!decide(curgt)(%d)\nre[curdep](%d) < ngrid(%d)\n\n", curdep, depth, !decide(curgt), re[curdep], ngrid);
        // 评估，当为计算的末节点则直接评估，否则按极大极小值赋值
        //printf("评估\n\n");
        if (curdep == depth || decide(curgt)) {
            curgt->score = evaluate(curgt);
            //printf("curgt->score:%d(0)\n\n", curgt->score);
        } else {
            re[curdep] = 0;
            //printf("re[%d] = 0\n\n", curdep);
            int val = curgt->branch[0]->score; // 值为首元素！
            for (int i = 0; i < curgt->nleaf; ++i) {
                //printf("curgt->branch[%d]->score:%d\n", i, curgt->branch[i]->score);
                if ((curdep + 1) % 2) {
                    // 奇数层，选最大
                    val = (curgt->branch[i]->score > val)
                              ? curgt->branch[i]->score
                              : val;
                } else {
                    // 偶数层，选最小
                    val = (curgt->branch[i]->score < val)
                              ? curgt->branch[i]->score
                              : val;
                }
            }
            curgt->score = val;
            //printf("curgt->score:%d(1)\n\n", curgt->score);
        }

        // 回溯到未得分节点
        //printf("回溯\n\n");
        while (curgt && curgt->score) {
            curgt = curgt->prev;
            --curdep;
            //printf("--curdep\n\n");
        }
        //printf("curdep:%d\n\n", curdep);

    } while (!root->score);

    return (void *)p;
}

int evaluate(gtree *gt) { //////

    return rand() % 1000;
}

int decide(gtree *gt) { // 判断结果(1结束，0未完成)，并赋值rate

    int(*p)[TS] = (int(*)[TS])gt->leaf;

    int i, j, n = -1;
    for (i = 0; i < TS; ++i) {
        for (j = 0; j < TS; ++j) {
            int sum;
            // row:
            if (j <= TS - CON) {
                for (int k = sum = 0; k < CON; ++k)
                    sum += p[i][j + k];
                if (abs(sum) == CON) goto end;
            }
            // col:
            if (i <= TS - CON) {
                for (int k = sum = 0; k < CON; ++k)
                    sum += p[i + k][j];
                if (abs(sum) == CON) goto end;
            }
            // sla:
            if (i >= CON - 1 && j <= TS - CON) {
                for (int k = sum = 0; k < CON; ++k)
                    sum += p[i - k][j + k];
                if (abs(sum) == CON) goto end;
            }
            // bsla:
            if (i <= TS - CON && j <= TS - CON) {
                for (int k = sum = 0; k < CON; ++k)
                    sum += p[i + k][j + k];
                if (abs(sum) == CON) goto end;
            }
            if (!p[i][j]) n = 0;
        }
    }
    return !!n;

end:
    // for (gtree *k = gt; k != NULL; k = k->prev)
    // k->score += p[i][j];
    return 1;
}

void *getNext(void *vprev, const long size, const int priority) {

    // 判断谁的回合，和为1则白，和为0则黑
    int *prev = (int *)memcpy(malloc(size), vprev, size);
    if (prev) m++;
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

gtree *generate() { // 广度优先生成博弈树

    int leaf[TS][TS] = {0};
    int nleaf = TS * TS;
    unsigned long long tnleaf = 1;
    gtree *root = gt_cre(leaf, sizeof(leaf));

    gtree **temp = (gtree **)malloc(sizeof(gtree *));
    if (temp) m++;
    *temp = root;
    gtree **curr = temp;
    gtree **alpha = NULL;

    while (nleaf && nleaf > TS * TS - GL) {
        alpha = (gtree **)calloc(1, (size_t)(sizeof(gtree *) * tnleaf * nleaf));
        if (alpha)
            m++;
        else
            exit(-222);

        for (unsigned long long j = 0, n = 0; j < tnleaf; ++j, ++curr)
            for (int i = 0; i < nleaf; ++i, ++n)
                if (*curr &&
                    (!(*curr)->prev ||
                     ((*curr)->prev && !decide(*curr)))) { // 在decide中赋值rate
                    void *next = getNext((*curr)->leaf, (*curr)->_sleaf, i + 1);
                    alpha[n] = gt_add(*curr, next, (*curr)->_sleaf);
                    if (next) f++;
                    free(next);
                }
        if (temp) f++;
        free(temp);
        curr = temp = alpha;
        tnleaf *= nleaf--;
    }
    return root;
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
    }
    char *p = NULL, *p1 = NULL;
    if (gt->prev) {
        if (gt == gt->prev->branch[gt->prev->nleaf - 1]) {
            p = "└─";
            p1 = "  ";
        } else {
            p = "├─";
            p1 = "│ ";
        }
        printf("%s%s %s\n", prefix, p, content);
    } else {
        p1 = "";
        printf("%s\n", content);
    }

    ///////////////////////////////
    int n = 0;
    for (int i = 1; i <= 2 * TS + 1; ++i) {
        printf("%s%s", prefix, p1);
        if (i % 2) {
            switch (i) {
            case 1:
                for (int j = 0; j < TS + 1; ++j)
                    switch (j) {
                    case 0:
                        printf("┌   ");
                        break;
                    case TS:
                        printf("┐ \n");
                        break;
                    default:
                        printf("┬   ");
                    }
                break;
            case 2 * TS + 1:
                for (int j = 0; j < TS + 1; ++j)
                    switch (j) {
                    case 0:
                        printf("└   ");
                        break;
                    case TS:
                        printf("┘ \n");
                        break;
                    default:
                        printf("┴   ");
                    }
                break;
            default:
                for (int j = 0; j < TS + 1; ++j)
                    switch (j) {
                    case 0:
                        printf("├   ");
                        break;
                    case TS:
                        printf("┤ \n");
                        break;
                    default:
                        printf("┼   ");
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

    gtree *gt = (gtree *)malloc(sizeof(gtree));
    if (gt) m++;
    if (!gt) return NULL;

    gt->prev = NULL;
    gt->branch = (gtree **)malloc(sizeof(gtree *) * INCECO);
    if (gt->branch) m++;
    gt->_nbranch = INCECO;
    gt->nleaf = 0;
    gt->score = 0;
    gt->alpha = 0;
    gt->beta = 0;
    gt->_sleaf = size;
    gt->leaf = malloc(size);
    if (gt->leaf) m++;
    if (!(gt->branch && gt->leaf)) {
        if (gt->branch) f++;
        free(gt->branch);
        if (gt->leaf) f++;
        free(gt->leaf);
        if (gt) f++;
        free(gt);
        return NULL;
    }
    memset(gt->branch, 0, sizeof(gtree *) * INCECO);
    memcpy(gt->leaf, leaf, size);
    ++k;
    return gt;
}

gtree *gt_add(gtree *const destgt, const void *leaf, const long size) {

    gtree *gt = gt_cre(leaf, size);
    if (!gt) return NULL;

    gt->prev = destgt;
    if (destgt->_nbranch <= destgt->nleaf) {
        gtree **temp = (gtree **)realloc(
            destgt->branch, sizeof(gtree *) * (destgt->_nbranch + INCECO));
        if (!temp) return NULL;
        memset(temp + destgt->_nbranch, 0, sizeof(gtree *) * INCECO);
        destgt->branch = temp;
        destgt->_nbranch += INCECO;
    }
    return destgt->branch[destgt->nleaf++] = gt;
}

int gt_del(gtree *gt) { // delete all behind gt

    // printf("\ndel:%p\n\n", gt);

    if (gt->prev) {
        for (int i = 0; i < gt->prev->nleaf; ++i) {
            if (gt->prev->branch[i] == gt) {
                gt->prev->branch[i] = NULL;
                if (i + 1 < gt->prev->nleaf) {
                    // printf("mov:dest %p, src %p, size:%d\n\n",
                    // gt->prev->branch + i, gt->prev->branch + i + 1,
                    // sizeof(gtree *) * (gt->prev->nleaf - i - 1));
                    memmove(gt->prev->branch + i, gt->prev->branch + i + 1,
                            sizeof(gtree *) * (gt->prev->nleaf - i - 1));
                }
                gt->prev->nleaf--;
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
    return (int)(n = iterations);
}