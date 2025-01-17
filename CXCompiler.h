#ifndef __CXCOMPILER_H__
#define __CXCOMPILER_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define bool int
#define true 1
#define false 0

#define norw 16       /* 保留字个数 */
#define txmax 100     /* 符号表容量 */
#define nmax 14       /* 数字的最大位数 */
#define al 10         /* 标识符的最大长度 */
#define maxerr 30     /* 允许的最多错误数 */
#define amax 2048     /* 地址上界*/
#define levmax 3      /* 最大允许过程嵌套声明层数*/
#define cxmax 200     /* 最多的虚拟机代码数 */
#define stacksize 500 /* 运行时数据栈元素最多为500个 */

/* 符号 */
enum symbol {
    nul,        ident,      number,     truesym,    falsesym,
    plus,       minus,      times,      slash,      lss,
    leq,        gtr,        geq,        eql,        neq,
    becomes,    orsym,      andsym,     not,        semicolon,
    lparen,     rparen,     lbrace,     rbrace,     mod,
    xorsym,     oddsym,     incre,      decre,      intsym,
    boolsym,    ifsym,      elsesym,    whilesym,   writesym,
    readsym,    forsym,     dosym,      constsym,   repeatsym,
    untilsym,   period,
};
#define symnum 42

/* 符号表中的类型 */
enum object {
    integer,
    booltype,
};

/* 虚拟机代码指令 */
enum fct {
    lit,     opr,     lod,
    sto,     ini,     jmp,
    jpc,     jeq,
};
#define fctnum 8

/* 虚拟机代码结构 */
struct instruction
{
	enum fct f; /* 虚拟机代码指令 */
	int l;      /* 引用层与声明层的层次差 */
	int a;      /* 根据f的不同而不同 */
};


char ch;            /* 存放当前读取的字符，getch 使用 */
enum symbol sym;    /* 当前的符号 */
char id[al+1];      /* 当前ident，多出的一个字节用于存放0 */
int num;            /* 当前number */
int cc, ll;         /* getch使用的计数器，cc表示当前字符(ch)的位置 */
int cx;             /* 虚拟机代码指针, 取值范围[0, cxmax-1]*/
int	dx;             /* 记录数据分配的相对地址 */
char line[81];      /* 读取行缓冲区 */
char a[al+1];       /* 临时符号，多出的一个字节用于存放0 */
struct instruction code[cxmax]; /* 存放虚拟机代码的数组 */
char word[norw][al];        /* 保留字 */
enum symbol wsym[norw];     /* 保留字对应的符号值 */
enum symbol ssym[256];      /* 单字符的符号值 */
char mnemonic[fctnum][5];   /* 虚拟机代码指令名称 */
bool declbegsys[symnum];    /* 表示声明开始的符号集合 */
bool statbegsys[symnum];    /* 表示语句开始的符号集合 */
bool facbegsys[symnum];     /* 表示因子开始的符号集合 */
int rel;
char errormsg[50][50];

/* 符号表结构 */
struct tablestruct
{
	char name[al];	    /* 名字 */
	enum object kind;	/* 类型：integer或booltype */
	bool bconst;        /* 是否为const */
	int val;            /* 数值，仅const使用 */
	int level;          /* 所处层，仅const不使用 */
	int adr;            /* 地址，仅const不使用 */
};

struct tablestruct table[txmax]; /* 符号表 */

FILE* fin;      /* 输入源文件 */
FILE* ftable;	/* 输出符号表 */
FILE* fcode;    /* 输出虚拟机代码 */
FILE* foutput;  /* 输出文件及出错示意（如有错）、各行对应的生成代码首地址（如无错） */
FILE* fresult;  /* 输出执行结果 */
char fname[al];
int err;        /* 错误计数器 */

void build(char* fname);
void error(int n);
void getsym();
void getch();
void init();
void gen(enum fct x, int y, int z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
void block(int lev, int tx, bool* fsys);
void interpret();
void factor(bool* fsys, int* ptx, int lev);
void term(bool* fsys, int* ptx, int lev);
void condition(bool* fsys, int* ptx, int lev);
void expression(bool* fsys, int* ptx, int lev);
void statement(bool* fsys, int* ptx, int lev);
void listall();
void vardeclaration(int* ptx, int lev, int* pdx);
void constdeclaration(int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
void enter(bool bconst, enum object k, int* ptx, int lev, int* pdx);
int base(int l, int* s, int b);

#endif
