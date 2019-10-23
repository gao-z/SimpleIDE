#ifndef __CXCOMPILER_H__
#define __CXCOMPILER_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define bool int
#define true 1
#define false 0

#define norw 16       /* �����ָ��� */
#define txmax 100     /* ���ű����� */
#define nmax 14       /* ���ֵ����λ�� */
#define al 10         /* ��ʶ������󳤶� */
#define maxerr 30     /* ������������� */
#define amax 2048     /* ��ַ�Ͻ�*/
#define levmax 3      /* ����������Ƕ����������*/
#define cxmax 200     /* ��������������� */
#define stacksize 500 /* ����ʱ����ջԪ�����Ϊ500�� */

/* ���� */
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

/* ���ű��е����� */
enum object {
    integer,
    booltype,
};

/* ���������ָ�� */
enum fct {
    lit,     opr,     lod,
    sto,     ini,     jmp,
    jpc,     jeq,
};
#define fctnum 8

/* ���������ṹ */
struct instruction
{
	enum fct f; /* ���������ָ�� */
	int l;      /* ���ò���������Ĳ�β� */
	int a;      /* ����f�Ĳ�ͬ����ͬ */
};


char ch;            /* ��ŵ�ǰ��ȡ���ַ���getch ʹ�� */
enum symbol sym;    /* ��ǰ�ķ��� */
char id[al+1];      /* ��ǰident�������һ���ֽ����ڴ��0 */
int num;            /* ��ǰnumber */
int cc, ll;         /* getchʹ�õļ�������cc��ʾ��ǰ�ַ�(ch)��λ�� */
int cx;             /* ���������ָ��, ȡֵ��Χ[0, cxmax-1]*/
int	dx;             /* ��¼���ݷ������Ե�ַ */
char line[81];      /* ��ȡ�л����� */
char a[al+1];       /* ��ʱ���ţ������һ���ֽ����ڴ��0 */
struct instruction code[cxmax]; /* ����������������� */
char word[norw][al];        /* ������ */
enum symbol wsym[norw];     /* �����ֶ�Ӧ�ķ���ֵ */
enum symbol ssym[256];      /* ���ַ��ķ���ֵ */
char mnemonic[fctnum][5];   /* ���������ָ������ */
bool declbegsys[symnum];    /* ��ʾ������ʼ�ķ��ż��� */
bool statbegsys[symnum];    /* ��ʾ��俪ʼ�ķ��ż��� */
bool facbegsys[symnum];     /* ��ʾ���ӿ�ʼ�ķ��ż��� */
int rel;
char errormsg[50][50];

/* ���ű�ṹ */
struct tablestruct
{
	char name[al];	    /* ���� */
	enum object kind;	/* ���ͣ�integer��booltype */
	bool bconst;        /* �Ƿ�Ϊconst */
	int val;            /* ��ֵ����constʹ�� */
	int level;          /* �����㣬��const��ʹ�� */
	int adr;            /* ��ַ����const��ʹ�� */
};

struct tablestruct table[txmax]; /* ���ű� */

FILE* fin;      /* ����Դ�ļ� */
FILE* ftable;	/* ������ű� */
FILE* fcode;    /* ������������ */
FILE* foutput;  /* ����ļ�������ʾ�⣨���д������ж�Ӧ�����ɴ����׵�ַ�����޴� */
FILE* fresult;  /* ���ִ�н�� */
char fname[al];
int err;        /* ��������� */

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
