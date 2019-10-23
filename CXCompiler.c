#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CXcompiler.h"

/* 主程序开始 */
void build(char* fname)
{
    bool nxtlev[symnum];

	if ((fin = fopen(fname, "r")) == NULL)
	{
		printf("Can't open the input file!\n");
		exit(1);
	}

	ch = fgetc(fin);
	if (ch == EOF)
	{
		printf("The input file is empty!\n");
		fclose(fin);
		exit(1);
	}
	rewind(fin);

	if ((foutput = fopen("foutput.txt", "w")) == NULL)
	{
		printf("Can't open the output file!\n");
		exit(1);
	}

	if ((ftable = fopen("ftable.txt", "w")) == NULL)
	{
		printf("Can't open ftable.txt file!\n");
		exit(1);
	}

    init();		/* 初始化 */
	err = 0;
	cc = ll = cx = 0;
	ch = ' ';
	dx = 3;
	rel = false;
    gen(ini, 0, dx);

	getsym();

	addset(nxtlev, declbegsys, statbegsys, symnum);
	nxtlev[period] = true;
	block(0, 0, nxtlev);	/* 处理分程序 */
    gen(opr, 0, 0);

	if (sym != period)
    {
		error(8);
    }

    if (err == 0)
    {
		printf("\n===编译成功!===\n");
		fprintf(foutput,"\n===编译成功!===\n");

		if ((fcode = fopen("fcode.txt", "w")) == NULL)
		{
			printf("Can't open fcode.txt file!\n");
			exit(1);
		}

		if ((fresult = fopen("fresult.txt", "w")) == NULL)
		{
			printf("Can't open fresult.txt file!\n");
			exit(1);
		}

		listall();	 /* 输出所有代码 */
		fclose(fcode);
    }
	else
	{
		printf("\n%d errors in CX program!\n",err);
		fprintf(foutput,"\n%d errors in CX program!\n",err);
	}

    fclose(ftable);
	fclose(foutput);
	fclose(fin);

}

/*
 * 初始化
 */
void init()
{
	int i;

	/* 设置单字符符号 */
	for (i=0; i<=255; i++)
	{
	    ssym[i] = nul;
	}
	ssym['*'] = times;
	ssym['%'] = mod;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['{'] = lbrace;
	ssym['}'] = rbrace;
	ssym[';'] = semicolon;
	ssym['#'] = period;

	/* 设置保留字名字,按照字母顺序，便于二分查找 */
	strcpy(&(word[0][0]), "bool");
	strcpy(&(word[1][0]), "const");
	strcpy(&(word[2][0]), "do");
	strcpy(&(word[3][0]), "else");
	strcpy(&(word[4][0]), "false");
	strcpy(&(word[5][0]), "for");
	strcpy(&(word[6][0]), "if");
	strcpy(&(word[7][0]), "int");
	strcpy(&(word[8][0]), "odd");
	strcpy(&(word[9][0]), "read");
	strcpy(&(word[10][0]), "repeat");
	strcpy(&(word[11][0]), "true");
	strcpy(&(word[12][0]), "until");
    strcpy(&(word[13][0]), "while");
    strcpy(&(word[14][0]), "write");
	strcpy(&(word[15][0]), "xor");

	/* 设置保留字符号 */
	wsym[0] = boolsym;
	wsym[1] = constsym;
	wsym[2] = dosym;
	wsym[3] = elsesym;
    wsym[4] = falsesym;
	wsym[5] = forsym;
	wsym[6] = ifsym;
	wsym[7] = intsym;
	wsym[8] = oddsym;
    wsym[9] = readsym;
    wsym[10] = repeatsym;
    wsym[11] = truesym;
    wsym[12] = untilsym;
	wsym[13] = whilesym;
	wsym[14] = writesym;
	wsym[15] = xorsym;

	/* 设置指令名称 */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[ini][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");
	strcpy(&(mnemonic[jeq][0]), "jeq");

    /* 设置符号集 */
	for (i=0; i<symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/* 设置声明开始符号集 */
	declbegsys[constsym] = true;
	declbegsys[intsym] = true;
	declbegsys[boolsym] = true;

	/* 设置语句开始符号集 */
	statbegsys[ident] = true;
	statbegsys[lbrace] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
    statbegsys[writesym] = true;
	statbegsys[readsym] = true;
	statbegsys[forsym] = true;
	statbegsys[dosym] = true;
	statbegsys[repeatsym] = true;

	/* 设置因子开始符号集 */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
	facbegsys[truesym] = true;
	facbegsys[falsesym] = true;

	/* 设置错误信息 */
    strcpy(errormsg[1], "要用“=”而不是“==”");
    strcpy(errormsg[2], "“=”之后必须跟随一个数");
    strcpy(errormsg[3], "“=”之后必须跟随一个布尔值");
    strcpy(errormsg[4], "这里必须是一个“=”");
    strcpy(errormsg[5], "这里必须是一个标识符");
    strcpy(errormsg[6], "丢了一个分号");
    strcpy(errormsg[7], "这里等待一条语句");
    strcpy(errormsg[8], "block语句部分之后出现的不正确符号");
    strcpy(errormsg[9], "该标识符没有说明");
    strcpy(errormsg[10], "给常量赋值是不允许的");
    strcpy(errormsg[11], "这里等待“{”");
    strcpy(errormsg[12], "这里等待“}”");
    strcpy(errormsg[13], "该语句跟着一个不正确的使用符号");
    strcpy(errormsg[14], "这里等待一个关系运算符");
    strcpy(errormsg[15], "丢了右括号");
    strcpy(errormsg[16], "丢了左括号");
    strcpy(errormsg[17], "表达式不能以此符号开始");
    strcpy(errormsg[18], "数字位数超过14位");
    strcpy(errormsg[19], "语句不完整");
    strcpy(errormsg[20], "要用“&&”而不是“&”");
    strcpy(errormsg[21], "要用“||”而不是“|”");
    strcpy(errormsg[22], "缺少正确的常量类型声明");
    strcpy(errormsg[23], "只有do没有while");
    strcpy(errormsg[24], "只有repeat没有until");
}

/*
 * 用数组实现集合的集合运算
 */
int inset(int e, bool* s)
{
	return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]||s2[i];
	}
	return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&(!s2[i]);
	}
	return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&s2[i];
	}
	return 0;
}

/*
 *	出错处理，打印出错位置和错误编码
 */
void error(int n)
{
	char space[81];
	memset(space,32,81);

	space[cc-1]=0; /* 出错时当前符号已经读完，所以cc-1 */

	printf("**%s^%s\n", space, errormsg[n]);
	fprintf(foutput,"**%s^%s\n", space, errormsg[n]);

	err = err + 1;
	if (err > maxerr)
	{
		exit(1);
	}
}

/*
 * 过滤空格，读取一个字符
 * 每次读一行，存入line缓冲区，line被getsym取空后再读一行
 * 被函数getsym调用
 */
void getch()
{
	if (cc == ll) /* 判断缓冲区中是否有字符，若无字符，则读入下一行字符到缓冲区中 */
	{
		ll = 0;
		cc = 0;
		printf("%d ", cx);
		fprintf(foutput,"%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
            if (EOF == fscanf(fin,"%c", &ch))
            {
                line[ll] = '#';
                printf("\n");
                fprintf(foutput, "\n");
                break;
            }

			printf("%c", ch);
			fprintf(foutput, "%c", ch);
			line[ll] = ch;
			ll++;
		}
		if (strstr(line, "==") || strstr(line, "!=") || strstr(line, "<") || strstr(line, "<=") || strstr(line, ">") || strstr(line, ">="))
        {
            rel = true;
        }
	}
	ch = line[cc];
	cc++;
}

/*
 * 词法分析，获取一个符号
 */
void getsym()
{
	int i,j,k;

	while (ch == ' ' || ch == 10 || ch == 9)	/* 过滤空格、换行和制表符 */
	{
		getch();
	}
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) /* 当前的单词是标识符或是保留字 */
	{
		k = 0;
		do {
			if(k < al)
			{
				a[k] = ch;
				k++;
			}
			getch();
		} while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));
		a[k] = 0;
		//strcpy(id, a);
		i = 0;
		j = norw - 1;
		do {    /* 搜索当前单词是否为保留字，使用二分法查找 */
			k = (i + j) / 2;
			if (strcmp(a,word[k]) <= 0)
			{
			    j = k - 1;
			}
			if (strcmp(a,word[k]) >= 0)
			{
			    i = k + 1;
			}
		} while (i <= j);
		if (i-1 > j) /* 当前的单词是保留字 */
		{
		    sym = wsym[k];
		}
		else /* 当前的单词是标识符 */
		{
		    sym = ident;
		    strcpy(id, a);
		}
	}
	else
	{
		if (ch >= '0' && ch <= '9') /* 当前的单词是数字 */
		{
			k = 0;
			num = 0;
			sym = number;
			do {
				num = 10 * num + ch - '0';
				k++;
				getch();;
			} while (ch >= '0' && ch <= '9'); /* 获取数字的值 */
			k--;
			if (k > nmax) /* 数字位数太多 */
			{
			    error(18);
			}
		}
		else
		{
			if (ch == '=')		/* 检测赋值符号 */
			{
				getch();
				if (ch == '=')
				{
					sym = eql;
					getch();
				}
				else
				{
					sym = becomes;
				}
			}
			else
			{
				if (ch == '<')		/* 检测小于或小于等于符号 */
				{
					getch();
					if (ch == '=')
					{
						sym = leq;
						getch();
					}
					else
					{
						sym = lss;
					}
				}
				else
				{
					if (ch == '>')		/* 检测大于或大于等于符号 */
					{
						getch();
						if (ch == '=')
						{
							sym = geq;
							getch();
						}
						else
						{
							sym = gtr;
						}
					}
					else
					{
						if (ch == '+')      /* 检测加号或自增符号 */
                        {
                            getch();
                            if (ch == '+')
                            {
                                sym = incre;
                                getch();
                            }
                            else
                            {
                                sym = plus;
                            }
                        }
                        else
                        {
                            if (ch == '-')      /* 检测减号或自减符号 */
                            {
                                getch();
                                if (ch == '-')
                                {
                                    sym = decre;
                                    getch();
                                }
                                else
                                {
                                    sym = minus;
                                }
                            }
                            else
                            {
                                if (ch == '!') /* 检测否定或不等符号 */
                                {
                                    getch();
                                    if (ch == '=')
                                    {
                                        sym = neq;
                                        getch();
                                    }
                                    else
                                    {
                                        sym = not;
                                    }
                                }
                                else
                                {
                                    if (ch == '/')
                                    {
                                        getch();
                                        if (ch == '*')
                                        {
                                            int flag = 0;
                                            while (ch != '#')
                                            {
                                                getch();
                                                if(ch == '*')
                                                {
                                                    flag = 1;
                                                }
                                                else if (ch == '/' && flag == 1)
                                                {
                                                    getch();
                                                    getsym();
                                                    return;
                                                }
                                                else
                                                {
                                                    flag = 0;
                                                }
                                            }
                                            if (ch == '#')
                                            {
                                                sym = period;
                                            }
                                        }
                                        else
                                        {
                                            sym = slash;
                                        }
                                    }
                                    else
                                    {
                                        if(ch == '&')
                                        {
                                            getch();
                                            if(ch == '&')
                                            {
                                                sym = andsym;
                                                getch();
                                            }
                                            else
                                            {
                                                error(20);
                                            }
                                        }
                                        else if(ch == '|')
                                        {
                                            getch();
                                            if(ch == '|')
                                            {
                                                sym = orsym;
                                                getch();
                                            }
                                            else
                                            {
                                                error(21);
                                            }
                                        }
                                        else
                                        {
                                            sym = ssym[ch];		/* 当符号不满足上述条件时，全部按照单字符符号处理 */
                                            if (sym != period)
                                            {
                                                getch();
                                            }
                                        }
                                    }
                                }
                            }
                        }
					}
				}
			}
		}
	}
}

/*
 * 生成虚拟机代码
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
void gen(enum fct x, int y, int z )
{
	if (cx >= cxmax)
	{
		printf("Program is too long!\n");	/* 生成的虚拟机代码程序过长 */
		exit(1);

	}
	if ( z >= amax)
	{
		printf("Displacement address is too big!\n");	/* 地址偏移越界 */
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
}


/*
 * 测试当前符号是否合法
 *
 * 在语法分析程序的入口和出口处调用测试函数test，
 * 检查当前单词进入和退出该语法单位的合法性
 *
 * s1:	需要的单词集合
 * s2:	如果不是需要的单词，在某一出错状态时，
 *      可恢复语法分析继续正常工作的补充单词符号集合
 * n:  	错误号
 */
void test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1))
	{
		error(n);
		/* 当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合 */
		while ((!inset(sym,s1)) && (!inset(sym,s2)))
		{
			getsym();
		}
	}
}

/*
 * 编译程序主体
 *
 * lev:    当前分程序所在层
 * tx:     符号表当前尾指针
 * fsys:   当前模块后继符号集合
 */
void block(int lev, int tx, bool* fsys)
{
	int i;

	int dx0;
	int tx0;                /* 保留初始tx */
	int cx0;                /* 保留初始cx */
	bool nxtlev[symnum];    /* 在下级函数的参数中，符号集合均为值参，但由于使用数组实现，
	                           传递进来的是指针，为防止下级函数改变上级函数的集合，开辟新的空间
	                           传递给下级函数*/

	dx0 = dx;
	tx0 = tx;		        /* 记录本层标识符的初始位置 */

    if (sym == lbrace)
    {
        getsym();
        do {
            while (inset(sym, declbegsys))
            {
                if (sym == constsym)	/* 遇到常量声明符号，开始处理常量声明 */
                {
                    getsym();
                    constdeclaration(&tx, lev, &dx);	/* dx的值会被constdeclaration改变，使用指针 */
                    if (sym == semicolon) /* 遇到分号结束定义常量 */
                    {
                        getsym();
                    }
                    else
                    {
                        error(6);   /* 漏掉了分号 */
                    }
                }
                else if (sym == intsym)		/* 遇到变量声明符号，开始处理变量声明 */
                {
                    getsym();
                    intdeclaration(&tx, lev, &dx);
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* 漏掉了分号 */
                    }
                }
                else if (sym == boolsym)		/* 遇到变量声明符号，开始处理变量声明 */
                {
                    getsym();
                    booldeclaration(&tx, lev, &dx);
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* 漏掉了分号 */
                    }
                }
            }
            memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
            nxtlev[semicolon] = true;
            nxtlev[rbrace] = true;
            test(nxtlev, declbegsys, 7);
        } while (inset(sym, declbegsys));	/* 直到没有声明符号 */

        cx0 = cx;
        gen(ini, 0, dx - dx0);	            /* 生成指令，此指令执行时在数据栈中为被调用的过程开辟dx个单元的数据区 */

        while (inset(sym, statbegsys) || sym == semicolon)
        {
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);	/* 每个后继符号集合都包含上层后继符号集合，以便补救 */
            nxtlev[rbrace] = true;
            nxtlev[semicolon] = true;
            statement(nxtlev, &tx, lev);
        }

        if (sym == rbrace)
        {
            getsym();
        }
        else
        {
            error(12);
        }

        memset(nxtlev, 0, sizeof(bool) * symnum);	/* 分程序没有补救集合 */
        test(fsys, nxtlev, 8);
    }

    if (tx0 == 0 && err == 0)
    {
        /* 输出符号表 */
        printf("\n===符号表===\n");
        fprintf(ftable,"\n===符号表===\n");
        for (i = 1; i <= tx; i++)
        {
            switch (table[i].kind)
            {
                case integer:
                    if (table[i].bconst)
                    {
                        printf("%d\t\const int\t%s\t", i, table[i].name);
                        printf("val=%d\n", table[i].val);
                        fprintf(ftable, "%d\t\const int\t%s\t", i, table[i].name);
                        fprintf(ftable, "val=%d\n", table[i].val);
                    }
                    else
                    {
                        printf("%d\t\      int\t%s\t", i, table[i].name);
                        printf("lev=%d\taddr=%d\n", table[i].level, table[i].adr);
                        fprintf(ftable, "%d\t\      int\t%s\t", i, table[i].name);
                        fprintf(ftable, "lev=%d\taddr=%d\n", table[i].level, table[i].adr);
                    }
                    break;
                case booltype:
                    if (table[i].bconst)
                    {
                        printf("%d\t\const bool\t%s\t", i, table[i].name);
                        fprintf(ftable, "%d\t\const bool\t%s\t", i, table[i].name);
                        printf("val=");
                        fprintf(ftable, "val=");
                        if (table[i].val)
                        {
                            printf("true\n");
                            fprintf(ftable, "true\n");
                        }
                        else
                        {
                            printf("false\n");
                            fprintf(ftable, "false\n");
                        }
                    }
                    else
                    {
                        printf("%d\t\      bool\t%s\t", i, table[i].name);
                        printf("lev=%d\taddr=%d\n", table[i].level, table[i].adr);
                        fprintf(ftable, "%d\t\      bool\t%s\t", i, table[i].name);
                        fprintf(ftable, "lev=%d\taddr=%d\n", table[i].level, table[i].adr);
                    }
                    break;
            }
        }
        printf("\n");
        fprintf(ftable,"\n");
    }
}

/*
 * 在符号表中加入一项
 *
 * k:      标识符的种类为const，var或procedure
 * ptx:    符号表尾指针的指针，为了可以改变符号表尾指针的值
 * lev:    标识符所在的层次
 * pdx:    dx为当前应分配的变量的相对地址，分配后要增加1
 *
 */
void enter(bool bconst, enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	table[(*ptx)].bconst = bconst;
	strcpy(table[(*ptx)].name, id); /* 符号表的name域记录标识符的名字 */
	table[(*ptx)].kind = k;
	switch (k)
	{
		case integer:
			if (bconst)     /* 常量 */
			{
                if (num > amax)
                {
                    error(18);	/* 常数越界 */
                    num = 0;
                }
                table[(*ptx)].val = num; /* 登记常数的值 */
			}
			else        /* 变量 */
            {
                table[(*ptx)].level = lev;
                table[(*ptx)].adr = (*pdx);
                (*pdx)++;
            }
			break;
		case booltype:
			if (bconst)     /* 常量 */
			{
                if (num != true && num != false)
                {
                    error(3);	/* 布尔值非法 */
                    num = 0;
                }
                table[(*ptx)].val = num; /* 登记布尔值 */
			}
			else        /* 变量 */
            {
                table[(*ptx)].level = lev;
                table[(*ptx)].adr = (*pdx);
                (*pdx)++;
            }
			break;
	}
}

/*
 * 查找标识符在符号表中的位置，从tx开始倒序查找标识符
 * 找到则返回在符号表中的位置，否则返回0
 *
 * id:    要查找的名字
 * tx:    当前符号表尾指针
 */
int position(char* id, int tx)
{
	int i;
	strcpy(table[0].name, id);
	i = tx;
	while (strcmp(table[i].name, id) != 0)
    {
        i--;
    }
	return i;
}

/*
 * 常量声明处理
 */
void constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == intsym)
	{
	    getsym();
        if (sym == ident)
        {
            getsym();
            if (sym == eql || sym == becomes)
            {
                if (sym == eql)
                {
                    error(1);	/* 把=写成了== */
                }
                getsym();
                if (sym == number)
                {
                    enter(true, integer, ptx, lev, pdx);
                    getsym();
                }
                else
                {
                    error(2);	/* 常量声明中的=后应是数字 */
                }
            }
            else
            {
                error(4);	/* 常量声明中的标识符后应是= */
            }
        }
        else
        {
            error(5);	/* 类型名后应是标识符 */
        }
	}
	else if (sym == boolsym)
    {
	    getsym();
        if (sym == ident)
        {
            getsym();
            if (sym == eql || sym == becomes)
            {
                if (sym == eql)
                {
                    error(1);	/* 要用“=”而不是“==” */
                }
                getsym();
                if (sym == truesym)
                {
                    num = true;
                    enter(true, booltype, ptx, lev, pdx);
                    getsym();
                }
                else if (sym == falsesym)
                {
                    num = false;
                    enter(true, booltype, ptx, lev, pdx);
                    getsym();
                }
                else
                {
                    error(3);	/* 常量声明中的=后应是ture或者false */
                }
            }
            else
            {
                error(4);	/* 常量声明中的标识符后应是= */
            }
        }
        else
        {
            error(5);	/* 类型名后应是标识符 */
        }
	}
	else
    {
        error(22);
    }
}

/*
 * 整型变量声明处理
 */
void intdeclaration(int* ptx,int lev,int* pdx)
{
    if (sym == ident)
    {
        enter(false, integer, ptx, lev, pdx);
        getsym();
	}
	else
    {
		error(5);	/* int后面应是标识符 */
    }
}

/*
 * 布尔变量声明处理
 */
void booldeclaration(int* ptx,int lev,int* pdx)
{
    if (sym == ident)
    {
        enter(false, booltype, ptx, lev, pdx);
        getsym();
	}
	else
    {
		error(5);	/* bool后面应是标识符 */
    }
}

/*
 * 输出所有目标代码
 */
void listall()
{
	int i;
    for (i = 0; i < cx; i++)
    {
        printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        fprintf(fcode,"%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
    }
}

/*
 * 语句处理
 */
void statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2, cx3, cx4, cx5, cx6, cx7;
	bool nxtlev[symnum];

	if (sym == ident)	/* 准备按照赋值语句处理 */
	{
		i = position(id, *ptx);/* 查找标识符在符号表中的位置 */
		if (i == 0)
		{
			error(9);	/* 标识符未声明 */
			getsym();
		}
		else
		{
			if(table[i].bconst)
            {
				error(10);	/* 赋值语句中，赋值号左部标识符应该是变量 */
				i = 0;
			}
            else
            {
                if(table[i].kind == integer)
                {
                    getsym();
                    if(sym == incre)
                    {
                        gen(lod, 0, table[i].adr);
                        gen(lit, 0, 1);
                        gen(opr, 0, 2);
                        gen(sto, 0, table[i].adr);
                        getsym();
                    }
                    else if(sym == decre)
                    {
                        gen(lod, 0, table[i].adr);
                        gen(lit, 0, 1);
                        gen(opr, 0, 3);
                        gen(sto, 0, table[i].adr);
                        getsym();
                    }
                    else if(sym == becomes)
                    {
                        getsym();
                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        expression(nxtlev, ptx, lev);	/* 处理赋值符号右侧表达式 */
                        if(i != 0)
                        {
                            /* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
                            gen(sto, lev-table[i].level, table[i].adr);
                        }
                    }
                    else
                    {
                        error(13);	/* 没有检测到赋值符号 */
                    }
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else if (sym != rparen)
                    {
                        error(6); /* 漏掉了分号 */
                    }
                }
                else
                {
                    getsym();
                    if(sym == becomes)
                    {
                        getsym();
                    }
                    else
                    {
                        error(13);
                    }
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    expression(nxtlev, ptx, lev);	/* 处理赋值符号右侧表达式 */
                    if(i != 0)
                    {
                        /* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
                        gen(sto, lev-table[i].level, table[i].adr);
                    }
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* 漏掉了分号 */
                    }
                }
            }
        }
	}
	else
	{
		if (sym == readsym)	/* 准备按照read语句处理 */
		{
			getsym();
            if (sym == ident)
            {
                i = position(id, *ptx);	/* 查找要读的变量 */
            }
            else
            {
                i = 0;
            }

            if (i == 0)
            {
                error(9);	/* read语句括号中的标识符应该是声明过的变量 */
            }
            else
            {
                gen(opr, 0, 16);	/* 生成输入指令，读取值到栈顶 */
                gen(sto, lev-table[i].level, table[i].adr);	/* 将栈顶内容送入变量单元中 */
            }
            getsym();
            if (sym == semicolon)
            {
                getsym();
            }
            else
            {
                error(6); /* 漏掉了分号 */
            }
		}
		else
		{
			if (sym == writesym)	/* 准备按照write语句处理 */
			{
                getsym();
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                gen(opr, 0, 14);	/* 生成输出指令，输出栈顶的值 */
                gen(opr, 0, 15);	/* 生成换行指令 */
                if (sym == semicolon)
                {
                    getsym();
                }
                else
                {
                    error(6); /* 漏掉了分号 */
                }
			}
			else
			{
                if (sym == ifsym)	/* 准备按照if语句处理 */
                {
                    getsym();
                    if (sym != lparen)
                    {
                        error(16);	/* 格式错误，应是左括号 */
                    }
                    else
                    {
                        getsym();
                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        nxtlev[rparen] = true;	/* 后继符号为{ */
                        if(rel == true)
                        {
                            condition(nxtlev, ptx, lev); /* 调用条件处理 */
                            rel = false;
                        }
                        else
                        {
                            expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                        }
                    }
                    if(sym != rparen)
                    {
                        error(15);	/* 格式错误，应是右括号 */
                        while (!inset(sym, fsys))	/* 出错补救，直到遇到上层函数的后继符号 */
                        {
                            getsym();
                        }
                    }
                    else
                    {
                        getsym();
                    }

                    cx1 = cx;	/* 保存当前指令地址 */
                    gen(jpc, 0, 0);	/* 生成条件跳转指令，跳转地址未知，暂时写0 */
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    nxtlev[elsesym] = true;	/* 后继符号为{ */
                    statement(nxtlev, ptx, lev);	/* 处理then后的语句 */
                    if (sym == elsesym)
                    {
                        getsym();
                        cx2 = cx;
                        gen(jmp, 0, 0);
                        code[cx1].a = cx;
                        statement(fsys, ptx, lev);
                        code[cx2].a = cx;
                    }
                    else
                    {
                        code[cx1].a = cx;
                    }
                }
                else
                {
                    if (sym == lbrace)	/* 准备按照复合语句处理 */
                    {
                        block(lev, *ptx, nxtlev); /* 递归调用 */
                    }
                    else
                    {
                        if (sym == whilesym)	/* 准备按照while语句处理 */
                        {
                            cx1 = cx;	/* 保存判断条件操作的位置 */
                            getsym();

                            if (sym != lparen)
                            {
                                error(16);	/* 格式错误，应是左括号 */
                            }
                            else
                            {
                                getsym();
                                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                if(rel == true)
                                {
                                    condition(nxtlev, ptx, lev); /* 调用条件处理 */
                                    rel = false;
                                }
                                else
                                {
                                    expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                                }
                                cx2 = cx;	/* 保存循环体的结束的下一个位置 */
                                gen(jpc, 0, 0);	/* 生成条件跳转，但跳出循环的地址未知，标记为0等待回填 */
                            }
                            if(sym != rparen)
                            {
                                error(15);	/* 格式错误，应是右括号 */
                                while (!inset(sym, fsys))	/* 出错补救，直到遇到上层函数的后继符号 */
                                {
                                    getsym();
                                }
                            }
                            else
                            {
                                getsym();
                            }

                            statement(fsys, ptx, lev);	/* 循环体 */
                            gen(jmp, 0, cx1);	/* 生成条件跳转指令，跳转到前面判断条件操作的位置 */
                            code[cx2].a = cx;	/* 回填跳出循环的地址 */
                        }
                        else
                        {
                            if (sym == forsym)
                            {
                                getsym();
                                if (sym != lparen)
                                {
                                    error(16);	/* 格式错误，应是左括号 */
                                }
                                else
                                {
                                    getsym();
                                    statement(fsys, ptx, lev);
                                    cx1 = cx;	/* 保存判断条件操作的位置 */
                                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                    if(rel == true)
                                    {
                                        condition(nxtlev, ptx, lev); /* 调用条件处理 */
                                        rel = false;
                                    }
                                    else
                                    {
                                        expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                                    }
                                    cx2 = cx;	/* 保存循环体的结束的下一个位置 */
                                    gen(jpc, 0, 0);	/* 生成条件跳转，但跳出循环的地址未知，标记为0等待回填 */
                                    getsym();
                                    cx3 = cx;
                                    gen(jmp, 0, 0);
                                    cx4 = cx;
                                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                    nxtlev[rparen] = true;
                                    statement(nxtlev, ptx, lev);
                                    cx5 = cx;
                                    gen(jmp, 0, 0);
                                }
                                if(sym != rparen)
                                {
                                    error(15);	/* 格式错误，应是右括号 */
                                    while (!inset(sym, fsys))	/* 出错补救，直到遇到上层函数的后继符号 */
                                    {
                                        getsym();
                                    }
                                }
                                else
                                {
                                    getsym();
                                }
                                cx6 = cx;
                                statement(fsys, ptx, lev);	/* 循环体 */
                                cx7 = cx;
                                gen(jmp, 0, 0);
                                gen(jmp, 0, cx1);	/* 生成条件跳转指令，跳转到前面判断条件操作的位置 */
                                code[cx3].a = cx6;
                                code[cx7].a = cx4;
                                code[cx5].a = cx1;
                                code[cx2].a = cx;	/* 回填跳出循环的地址 */
                            }
                            else
                            {
                                if (sym == dosym)
                                {
                                    getsym();
                                    if (sym == lbrace)
                                    {
                                        cx1 = cx;
                                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                        nxtlev[semicolon] = true;
                                        block(lev, *ptx, nxtlev); /* 递归调用 */
                                    }
                                    else
                                    {
                                        error(11);
                                    }
                                    if (sym == whilesym)
                                    {
                                        getsym();
                                        if (sym != lparen)
                                        {
                                            error(16);	/* 格式错误，应是左括号 */
                                        }
                                        else
                                        {
                                            getsym();
                                            cx2 = cx;
                                            if(rel == true)
                                            {
                                                condition(nxtlev, ptx, lev); /* 调用条件处理 */
                                                rel = false;
                                            }
                                            else
                                            {
                                                expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                                            }
                                            cx3 = cx;
                                            gen(jeq, 0, 0);	/* 生成条件跳转，但跳出循环的地址未知，标记为0等待回填 */
                                            code[cx3].a = cx1;
                                        }
                                        if(sym != rparen)
                                        {
                                            error(15);	/* 格式错误，应是右括号 */
                                            while (!inset(sym, fsys))	/* 出错补救，直到遇到上层函数的后继符号 */
                                            {
                                                getsym();
                                            }
                                        }
                                        else
                                        {
                                            getsym();
                                        }
                                        if(sym != semicolon)
                                        {
                                            error(6);
                                        }
                                        else
                                        {
                                            getsym();
                                        }
                                    }
                                    else
                                    {
                                        error(23);
                                    }
                                }
                                else if (sym == repeatsym)
                                {
                                    getsym();
                                    if (sym == lbrace)
                                    {
                                        cx1 = cx;
                                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                        nxtlev[semicolon] = true;
                                        nxtlev[untilsym] = true;
                                        block(lev, *ptx, nxtlev); /* 递归调用 */
                                    }
                                    else
                                    {
                                        error(11);
                                    }
                                    if (sym == untilsym)
                                    {
                                        getsym();
                                        if (sym != lparen)
                                        {
                                            error(16);	/* 格式错误，应是左括号 */
                                        }
                                        else
                                        {
                                            getsym();
                                            cx2 = cx;
                                            if(rel == true)
                                            {
                                                condition(nxtlev, ptx, lev); /* 调用条件处理 */
                                                rel = false;
                                            }
                                            else
                                            {
                                                expression(nxtlev, ptx, lev);	/* 调用表达式处理 */
                                            }
                                            cx3 = cx;
                                            gen(jpc, 0, 0);	/* 生成条件跳转，但跳出循环的地址未知，标记为0等待回填 */
                                            code[cx3].a = cx1;
                                        }
                                        if(sym != rparen)
                                        {
                                            error(15);	/* 格式错误，应是右括号 */
                                            while (!inset(sym, fsys))	/* 出错补救，直到遇到上层函数的后继符号 */
                                            {
                                                getsym();
                                            }
                                        }
                                        else
                                        {
                                            getsym();
                                        }
                                        if(sym != semicolon)
                                        {
                                            error(6);
                                        }
                                        else
                                        {
                                            getsym();
                                        }
                                    }
                                    else
                                    {
                                        error(24);
                                    }
                                }
                                else
                                {
                                    if (sym == semicolon)
                                    {
                                        getsym();
                                    }
                                }
                            }
                        }
                    }
                }
			}
		}
	}
	memset(nxtlev, 0, sizeof(bool) * symnum);	/* 语句结束无补救集合 */
	test(fsys, nxtlev, 19);	/* 检测语句结束的正确性 */
}

/*
 * 表达式处理
 */
void expression(bool* fsys, int* ptx, int lev)
{
	enum symbol addop;	/* 用于保存正负号 */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[plus] = true;
	nxtlev[minus] = true;
	term(nxtlev, ptx, lev);	/* 处理项 */
	while (sym == plus || sym == minus || sym == orsym || sym == xorsym)
	{
		addop = sym;
		getsym();
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		term(nxtlev, ptx, lev);	/* 处理项 */
		if (addop == plus)
		{
			gen(opr, 0, 2);	/* 生成加法指令 */
		}
		else if (addop == minus)
		{
			gen(opr, 0, 3);	/* 生成减法指令 */
		}
		else if (addop == orsym)
		{
			gen(opr, 0, 18);	/* 生成逻辑或运算指令 */
		}
		else if (addop == xorsym)
		{
			gen(opr, 0, 20);	/* 生成异或运算指令 */
		}
	}
}

/*
 * 项处理
 */
void term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop;	/* 用于保存乘除法符号 */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	nxtlev[mod] = true;
	nxtlev[xorsym] = true;
	factor(nxtlev, ptx, lev);	/* 处理因子 */
	while(sym == times || sym == slash || sym == mod || sym == andsym)
	{
		mulop = sym;
		getsym();
		factor(nxtlev, ptx, lev);
		if(mulop == times)
		{
			gen(opr, 0, 4);	/* 生成乘法指令 */
		}
		else if(mulop == slash)
		{
			gen(opr, 0, 5);	/* 生成除法指令 */
		}
		else if(mulop == mod)
        {
            gen(opr, 0, 7); /* 生成求余指令*/
        }
        else if(mulop == andsym)
        {
            gen(opr, 0, 17); /* 生成逻辑与运算指令 */
        }
	}
}

/*
 * 因子处理
 */
void factor(bool* fsys, int* ptx, int lev)
{
	int i;
	bool nxtlev[symnum];
	//test(facbegsys, fsys, 24);	/* 检测因子的开始符号 */

	if (sym == not)
    {
        getsym();
        factor(nxtlev, ptx, lev);
        gen(opr, 0, 19);
    }
	while(inset(sym, facbegsys)) 	/* 循环处理因子 */
	{
		if(sym == ident)	/* 因子为常量或变量 */
		{
			i = position(id, *ptx);	/* 查找标识符在符号表中的位置 */
			if (i == 0)
			{
				error(9);	/* 标识符未声明 */
				getsym();
			}
			else
			{
                if (table[i].bconst)	/* 标识符为常量 */
                {
                    gen(lit, 0, table[i].val);	/* 直接把常量的值入栈 */
                }
                else
                {
                    gen(lod, lev-table[i].level, table[i].adr);	/* 找到变量地址并将其值入栈 */
                }
                getsym();
			}
		}
		else
		{
			if(sym == number)	/* 因子为数 */
			{
				if (num > amax)
				{
					error(18); /* 数越界 */
					num = 0;
				}
				gen(lit, 0, num);
				getsym();
			}
			else
			{
				if (sym == lparen)	/* 因子为表达式 */
				{
					getsym();
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					nxtlev[rparen] = true;
					expression(nxtlev, ptx, lev);
					if (sym == rparen)
					{
						getsym();
					}
					else
					{
						error(16);	/* 缺少右括号 */
					}
				}
				else
                {
                    if (sym == truesym)
                    {
                        gen(lit, 0, true);
                    }
                    else if (sym == falsesym)
                    {
                        gen(lit, 0, false);
                    }
                    getsym();
                }
			}
		}
		memset(nxtlev, 0, sizeof(bool) * symnum);
		nxtlev[lparen] = true;
		//test(fsys, nxtlev, 23); /* 一个因子处理完毕，遇到的单词应在fsys集合中 */
		                        /* 如果不是，报错并找到下一个因子的开始，使语法分析可以继续运行下去 */
	}
}

/*
 * 条件处理
 */
void condition(bool* fsys, int* ptx, int lev)
{
	int i;
	enum symbol relop;
	bool nxtlev[symnum];

	if(sym == oddsym)	/* 准备按照odd运算处理 */
	{
		getsym();
		if (sym == lparen)	/* 因子为表达式 */
        {
            getsym();
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[rparen] = true;
            expression(nxtlev, ptx, lev);
            gen(opr, 0, 6);	/* 生成odd指令 */
            if (sym == rparen)
            {
                getsym();
            }
            else
            {
                error(15);	/* 缺少右括号 */
            }
        }
	}
	else
	{
		/* 逻辑表达式处理 */
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[eql] = true;
		nxtlev[neq] = true;
		nxtlev[lss] = true;
		nxtlev[leq] = true;
		nxtlev[gtr] = true;
		nxtlev[geq] = true;
		expression(fsys, ptx, lev);
		if (sym == rparen)
        {
            return;
        }
        else if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq)
		{
			error(14); /* 应该为关系运算符 */
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys, ptx, lev);
			switch (relop)
			{
				case eql:
					gen(opr, 0, 8);
					break;
				case neq:
					gen(opr, 0, 9);
					break;
				case lss:
					gen(opr, 0, 10);
					break;
				case geq:
					gen(opr, 0, 11);
					break;
				case gtr:
					gen(opr, 0, 12);
					break;
				case leq:
					gen(opr, 0, 13);
					break;
			}
		}
	}
}

/*
 * 解释程序
 */
void interpret()
{
    int n=0, no;
    char type[3];

	if ((fin = fopen("fcode.txt", "r")) == NULL)
	{
		printf("Can't open the code file!\n");
		exit(1);
	}

	if ((fresult = fopen("fresult.txt", "w")) == NULL)
    {
        printf("Can't open fresult.txt file!\n");
        exit(1);
    }

	int p = 0; /* 指令指针 */
	int b = 1; /* 指令基址 */
	int t = 0; /* 栈顶指针 */
	struct instruction i;	/* 存放当前指令 */
	int s[stacksize];	/* 栈 */

	while (fscanf(fin,"%d%s%d%d", &no, &type, &code[n].l, &code[n].a) != EOF)
    {
        if (strcmp(type,"lit")==0)
            code[n].f = 0;
        else if (strcmp(type,"opr")==0)
            code[n].f = 1;
        else if (strcmp(type,"lod")==0)
            code[n].f = 2;
        else if (strcmp(type,"sto")==0)
            code[n].f = 3;
        else if (strcmp(type,"int")==0)
            code[n].f = 4;
        else if (strcmp(type,"jmp")==0)
            code[n].f = 5;
        else if (strcmp(type,"jpc")==0)
            code[n].f = 6;
        else if (strcmp(type,"jeq")==0)
            code[n].f = 7;
        n++;
    }

	printf("\n===开始运行...===\n");
	fprintf(fresult,"\n===开始运行...===\n");
	s[0] = 0; /* s[0]不用 */
	s[1] = 0; /* 主程序的三个联系单元均置为0 */
	s[2] = 0;
	s[3] = 0;
	do {
	    i = code[p];	/* 读当前指令 */
		p = p + 1;
		switch (i.f)
		{
			case lit:	/* 将常量a的值取到栈顶 */
				t = t + 1;
				s[t] = i.a;
				break;
			case opr:	/* 数学、逻辑运算 */
				switch (i.a)
				{
					case 0:  /* 函数调用结束后返回 */
						t = b - 1;
						p = s[t + 3];
						b = s[t + 2];
						break;
					case 1: /* 栈顶元素取反 */
						s[t] = - s[t];
						break;
					case 2: /* 次栈顶项加上栈顶项，退两个栈元素，相加值进栈 */
						t = t - 1;
						s[t] = s[t] + s[t + 1];
						break;
					case 3:/* 次栈顶项减去栈顶项 */
						t = t - 1;
						s[t] = s[t] - s[t + 1];
						break;
					case 4:/* 次栈顶项乘以栈顶项 */
						t = t - 1;
						s[t] = s[t] * s[t + 1];
						break;
					case 5:/* 次栈顶项除以栈顶项 */
						t = t - 1;
						s[t] = s[t] / s[t + 1];
						break;
					case 6:/* 栈顶元素的奇偶判断 */
						s[t] = s[t] % 2;
						break;
                    case 7:/* 次栈顶项模以栈顶项 */
						t = t - 1;
						s[t] = s[t] % s[t + 1];
						break;
					case 8:/* 次栈顶项与栈顶项是否相等 */
						t = t - 1;
						s[t] = (s[t] == s[t + 1]);
						break;
					case 9:/* 次栈顶项与栈顶项是否不等 */
						t = t - 1;
						s[t] = (s[t] != s[t + 1]);
						break;
					case 10:/* 次栈顶项是否小于栈顶项 */
						t = t - 1;
						s[t] = (s[t] < s[t + 1]);
						break;
					case 11:/* 次栈顶项是否大于等于栈顶项 */
						t = t - 1;
						s[t] = (s[t] >= s[t + 1]);
						break;
					case 12:/* 次栈顶项是否大于栈顶项 */
						t = t - 1;
						s[t] = (s[t] > s[t + 1]);
						break;
					case 13: /* 次栈顶项是否小于等于栈顶项 */
						t = t - 1;
						s[t] = (s[t] <= s[t + 1]);
						break;
					case 14:/* 栈顶值输出 */
						printf("%d", s[t]);
						fprintf(fresult, "%d", s[t]);
						t = t - 1;
						break;
					case 15:/* 输出换行符 */
						printf("\n");
						fprintf(fresult,"\n");
						break;
					case 16:/* 读入一个输入置于栈顶 */
						t = t + 1;
						printf("?");
						fprintf(fresult, "?");
						scanf("%d", &(s[t]));
						fprintf(fresult, "%d\n", s[t]);
						break;
                    case 17:/* 次栈顶项与栈顶项进行逻辑与运算 */
                        t = t - 1;
						s[t] = (s[t] & s[t + 1]);
						break;
                    case 18:/* 次栈顶项与栈顶项进行逻辑或运算 */
                        t = t - 1;
						s[t] = (s[t] | s[t + 1]);
						break;
                    case 19:/* 栈顶元素逻辑取反 */
						s[t] = s[t] ^ 1;
						break;
                    case 20:/* 次栈顶项与栈顶项进行异或运算 */
                        t = t - 1;
						s[t] = (s[t] ^ s[t + 1]);
						break;
				}
				break;
			case lod:	/* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
				t = t + 1;
				s[t] = s[base(i.l,s,b) + i.a];
				break;
			case sto:	/* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
				s[base(i.l, s, b) + i.a] = s[t];
				t = t - 1;
				break;
			case ini:	/* 在数据栈中为被调用的过程开辟a个单元的数据区 */
				t = t + i.a;
				break;
			case jmp:	/* 直接跳转 */
				p = i.a;
				break;
			case jpc:	/* 条件跳转 */
				if (s[t] == 0)
					p = i.a;
				t = t - 1;
				break;
            case jeq:	/* 条件跳转 */
				if (s[t] != 0)
					p = i.a;
				t = t - 1;
				break;
		}
	} while (p != 0);
	printf("\n===运行结束===\n\n");
	fprintf(fresult,"\n===运行结束===\n");
    fclose(fresult);
    fclose(fin);
}

/* 通过过程基址求上l层过程的基址 */
int base(int l, int* s, int b)
{
	int b1;
	b1 = b;
	while (l > 0)
	{
		b1 = s[b1];
		l--;
	}
	return b1;
}

/*
 * 调试程序
 */
void debug()
{
    int n=0, no;
    char type[3];

	if ((fin = fopen("fcode.txt", "r")) == NULL)
	{
		printf("Can't open the code file!\n");
		exit(1);
	}

	int p = 0; /* 指令指针 */
	int b = 1; /* 指令基址 */
	int t = 0; /* 栈顶指针 */
	struct instruction i;	/* 存放当前指令 */
	int s[stacksize];	/* 栈 */

	while (fscanf(fin,"%d%s%d%d", &no, &type, &code[n].l, &code[n].a) != EOF)
    {
        if (strcmp(type,"lit")==0)
            code[n].f = 0;
        else if (strcmp(type,"opr")==0)
            code[n].f = 1;
        else if (strcmp(type,"lod")==0)
            code[n].f = 2;
        else if (strcmp(type,"sto")==0)
            code[n].f = 3;
        else if (strcmp(type,"int")==0)
            code[n].f = 4;
        else if (strcmp(type,"jmp")==0)
            code[n].f = 5;
        else if (strcmp(type,"jpc")==0)
            code[n].f = 6;
        else if (strcmp(type,"jeq")==0)
            code[n].f = 7;
        n++;
    }

	printf("\n===开始调试...===\n");
	s[0] = 0; /* s[0]不用 */
	s[1] = 0; /* 主程序的三个联系单元均置为0 */
	s[2] = 0;
	s[3] = 0;
	do {
	    i = code[p];	/* 读当前指令 */
		p = p + 1;
		switch (i.f)
		{
			case lit:	/* 将常量a的值取到栈顶 */
				t = t + 1;
				s[t] = i.a;
				break;
			case opr:	/* 数学、逻辑运算 */
				switch (i.a)
				{
					case 0:  /* 函数调用结束后返回 */
						t = b - 1;
						p = s[t + 3];
						b = s[t + 2];
						break;
					case 1: /* 栈顶元素取反 */
						s[t] = - s[t];
						break;
					case 2: /* 次栈顶项加上栈顶项，退两个栈元素，相加值进栈 */
						t = t - 1;
						s[t] = s[t] + s[t + 1];
						break;
					case 3:/* 次栈顶项减去栈顶项 */
						t = t - 1;
						s[t] = s[t] - s[t + 1];
						break;
					case 4:/* 次栈顶项乘以栈顶项 */
						t = t - 1;
						s[t] = s[t] * s[t + 1];
						break;
					case 5:/* 次栈顶项除以栈顶项 */
						t = t - 1;
						s[t] = s[t] / s[t + 1];
						break;
					case 6:/* 栈顶元素的奇偶判断 */
						s[t] = s[t] % 2;
						break;
                    case 7:/* 次栈顶项模以栈顶项 */
						t = t - 1;
						s[t] = s[t] % s[t + 1];
						break;
					case 8:/* 次栈顶项与栈顶项是否相等 */
						t = t - 1;
						s[t] = (s[t] == s[t + 1]);
						break;
					case 9:/* 次栈顶项与栈顶项是否不等 */
						t = t - 1;
						s[t] = (s[t] != s[t + 1]);
						break;
					case 10:/* 次栈顶项是否小于栈顶项 */
						t = t - 1;
						s[t] = (s[t] < s[t + 1]);
						break;
					case 11:/* 次栈顶项是否大于等于栈顶项 */
						t = t - 1;
						s[t] = (s[t] >= s[t + 1]);
						break;
					case 12:/* 次栈顶项是否大于栈顶项 */
						t = t - 1;
						s[t] = (s[t] > s[t + 1]);
						break;
					case 13: /* 次栈顶项是否小于等于栈顶项 */
						t = t - 1;
						s[t] = (s[t] <= s[t + 1]);
						break;
					case 14:/* 栈顶值输出 */
						printf("%d", s[t]);
						fprintf(fresult, "%d", s[t]);
						t = t - 1;
						break;
					case 15:/* 输出换行符 */
						printf("\n");
						fprintf(fresult,"\n");
						break;
					case 16:/* 读入一个输入置于栈顶 */
						t = t + 1;
						printf("?");
						fprintf(fresult, "?");
						scanf("%d", &(s[t]));
						fprintf(fresult, "%d\n", s[t]);
						break;
                    case 17:/* 次栈顶项与栈顶项进行逻辑与运算 */
                        t = t - 1;
						s[t] = (s[t] & s[t + 1]);
						break;
                    case 18:/* 次栈顶项与栈顶项进行逻辑或运算 */
                        t = t - 1;
						s[t] = (s[t] | s[t + 1]);
						break;
                    case 19:/* 栈顶元素逻辑取反 */
						s[t] = s[t] ^ 1;
						break;
                    case 20:/* 次栈顶项与栈顶项进行异或运算 */
                        t = t - 1;
						s[t] = (s[t] ^ s[t + 1]);
						break;
				}
				break;
			case lod:	/* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
				t = t + 1;
				s[t] = s[base(i.l,s,b) + i.a];
				break;
			case sto:	/* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
				s[base(i.l, s, b) + i.a] = s[t];
				t = t - 1;
				break;
			case ini:	/* 在数据栈中为被调用的过程开辟a个单元的数据区 */
				t = t + i.a;
				break;
			case jmp:	/* 直接跳转 */
				p = i.a;
				break;
			case jpc:	/* 条件跳转 */
				if (s[t] == 0)
					p = i.a;
				t = t - 1;
				break;
            case jeq:	/* 条件跳转 */
				if (s[t] != 0)
					p = i.a;
				t = t - 1;
				break;
		}
		printf("当前指令：%s %d %d\n", mnemonic[i.f], i.l, i.a);
		printf("===数据栈===\n");
		int j;
		for(j = t; j >= 0; j--)
            printf("%d\t%d\n", j, s[j]);
		printf("按回车键运行下一条指令...\n");
		while(getchar() == '\r')
            break;
	} while (p != 0);
	printf("\n===运行结束===\n\n");
    fclose(fin);
}
