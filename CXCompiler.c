#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CXcompiler.h"

/* ������ʼ */
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

    init();		/* ��ʼ�� */
	err = 0;
	cc = ll = cx = 0;
	ch = ' ';
	dx = 3;
	rel = false;
    gen(ini, 0, dx);

	getsym();

	addset(nxtlev, declbegsys, statbegsys, symnum);
	nxtlev[period] = true;
	block(0, 0, nxtlev);	/* ����ֳ��� */
    gen(opr, 0, 0);

	if (sym != period)
    {
		error(8);
    }

    if (err == 0)
    {
		printf("\n===����ɹ�!===\n");
		fprintf(foutput,"\n===����ɹ�!===\n");

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

		listall();	 /* ������д��� */
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
 * ��ʼ��
 */
void init()
{
	int i;

	/* ���õ��ַ����� */
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

	/* ���ñ���������,������ĸ˳�򣬱��ڶ��ֲ��� */
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

	/* ���ñ����ַ��� */
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

	/* ����ָ������ */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[ini][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");
	strcpy(&(mnemonic[jeq][0]), "jeq");

    /* ���÷��ż� */
	for (i=0; i<symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/* ����������ʼ���ż� */
	declbegsys[constsym] = true;
	declbegsys[intsym] = true;
	declbegsys[boolsym] = true;

	/* ������俪ʼ���ż� */
	statbegsys[ident] = true;
	statbegsys[lbrace] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
    statbegsys[writesym] = true;
	statbegsys[readsym] = true;
	statbegsys[forsym] = true;
	statbegsys[dosym] = true;
	statbegsys[repeatsym] = true;

	/* �������ӿ�ʼ���ż� */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
	facbegsys[truesym] = true;
	facbegsys[falsesym] = true;

	/* ���ô�����Ϣ */
    strcpy(errormsg[1], "Ҫ�á�=�������ǡ�==��");
    strcpy(errormsg[2], "��=��֮��������һ����");
    strcpy(errormsg[3], "��=��֮��������һ������ֵ");
    strcpy(errormsg[4], "���������һ����=��");
    strcpy(errormsg[5], "���������һ����ʶ��");
    strcpy(errormsg[6], "����һ���ֺ�");
    strcpy(errormsg[7], "����ȴ�һ�����");
    strcpy(errormsg[8], "block��䲿��֮����ֵĲ���ȷ����");
    strcpy(errormsg[9], "�ñ�ʶ��û��˵��");
    strcpy(errormsg[10], "��������ֵ�ǲ������");
    strcpy(errormsg[11], "����ȴ���{��");
    strcpy(errormsg[12], "����ȴ���}��");
    strcpy(errormsg[13], "��������һ������ȷ��ʹ�÷���");
    strcpy(errormsg[14], "����ȴ�һ����ϵ�����");
    strcpy(errormsg[15], "����������");
    strcpy(errormsg[16], "����������");
    strcpy(errormsg[17], "���ʽ�����Դ˷��ſ�ʼ");
    strcpy(errormsg[18], "����λ������14λ");
    strcpy(errormsg[19], "��䲻����");
    strcpy(errormsg[20], "Ҫ�á�&&�������ǡ�&��");
    strcpy(errormsg[21], "Ҫ�á�||�������ǡ�|��");
    strcpy(errormsg[22], "ȱ����ȷ�ĳ�����������");
    strcpy(errormsg[23], "ֻ��doû��while");
    strcpy(errormsg[24], "ֻ��repeatû��until");
}

/*
 * ������ʵ�ּ��ϵļ�������
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
 *	��������ӡ����λ�úʹ������
 */
void error(int n)
{
	char space[81];
	memset(space,32,81);

	space[cc-1]=0; /* ����ʱ��ǰ�����Ѿ����꣬����cc-1 */

	printf("**%s^%s\n", space, errormsg[n]);
	fprintf(foutput,"**%s^%s\n", space, errormsg[n]);

	err = err + 1;
	if (err > maxerr)
	{
		exit(1);
	}
}

/*
 * ���˿ո񣬶�ȡһ���ַ�
 * ÿ�ζ�һ�У�����line��������line��getsymȡ�պ��ٶ�һ��
 * ������getsym����
 */
void getch()
{
	if (cc == ll) /* �жϻ��������Ƿ����ַ��������ַ����������һ���ַ����������� */
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
 * �ʷ���������ȡһ������
 */
void getsym()
{
	int i,j,k;

	while (ch == ' ' || ch == 10 || ch == 9)	/* ���˿ո񡢻��к��Ʊ�� */
	{
		getch();
	}
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) /* ��ǰ�ĵ����Ǳ�ʶ�����Ǳ����� */
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
		do {    /* ������ǰ�����Ƿ�Ϊ�����֣�ʹ�ö��ַ����� */
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
		if (i-1 > j) /* ��ǰ�ĵ����Ǳ����� */
		{
		    sym = wsym[k];
		}
		else /* ��ǰ�ĵ����Ǳ�ʶ�� */
		{
		    sym = ident;
		    strcpy(id, a);
		}
	}
	else
	{
		if (ch >= '0' && ch <= '9') /* ��ǰ�ĵ��������� */
		{
			k = 0;
			num = 0;
			sym = number;
			do {
				num = 10 * num + ch - '0';
				k++;
				getch();;
			} while (ch >= '0' && ch <= '9'); /* ��ȡ���ֵ�ֵ */
			k--;
			if (k > nmax) /* ����λ��̫�� */
			{
			    error(18);
			}
		}
		else
		{
			if (ch == '=')		/* ��⸳ֵ���� */
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
				if (ch == '<')		/* ���С�ڻ�С�ڵ��ڷ��� */
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
					if (ch == '>')		/* �����ڻ���ڵ��ڷ��� */
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
						if (ch == '+')      /* ���ӺŻ��������� */
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
                            if (ch == '-')      /* �����Ż��Լ����� */
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
                                if (ch == '!') /* ���񶨻򲻵ȷ��� */
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
                                            sym = ssym[ch];		/* �����Ų�������������ʱ��ȫ�����յ��ַ����Ŵ��� */
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
 * �������������
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
void gen(enum fct x, int y, int z )
{
	if (cx >= cxmax)
	{
		printf("Program is too long!\n");	/* ���ɵ���������������� */
		exit(1);

	}
	if ( z >= amax)
	{
		printf("Displacement address is too big!\n");	/* ��ַƫ��Խ�� */
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
}


/*
 * ���Ե�ǰ�����Ƿ�Ϸ�
 *
 * ���﷨�����������ںͳ��ڴ����ò��Ժ���test��
 * ��鵱ǰ���ʽ�����˳����﷨��λ�ĺϷ���
 *
 * s1:	��Ҫ�ĵ��ʼ���
 * s2:	���������Ҫ�ĵ��ʣ���ĳһ����״̬ʱ��
 *      �ɻָ��﷨�����������������Ĳ��䵥�ʷ��ż���
 * n:  	�����
 */
void test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1))
	{
		error(n);
		/* ����ⲻͨ��ʱ����ͣ��ȡ���ţ�ֱ����������Ҫ�ļ��ϻ򲹾ȵļ��� */
		while ((!inset(sym,s1)) && (!inset(sym,s2)))
		{
			getsym();
		}
	}
}

/*
 * �����������
 *
 * lev:    ��ǰ�ֳ������ڲ�
 * tx:     ���ű�ǰβָ��
 * fsys:   ��ǰģ���̷��ż���
 */
void block(int lev, int tx, bool* fsys)
{
	int i;

	int dx0;
	int tx0;                /* ������ʼtx */
	int cx0;                /* ������ʼcx */
	bool nxtlev[symnum];    /* ���¼������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������ʵ�֣�
	                           ���ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ������ļ��ϣ������µĿռ�
	                           ���ݸ��¼�����*/

	dx0 = dx;
	tx0 = tx;		        /* ��¼�����ʶ���ĳ�ʼλ�� */

    if (sym == lbrace)
    {
        getsym();
        do {
            while (inset(sym, declbegsys))
            {
                if (sym == constsym)	/* ���������������ţ���ʼ���������� */
                {
                    getsym();
                    constdeclaration(&tx, lev, &dx);	/* dx��ֵ�ᱻconstdeclaration�ı䣬ʹ��ָ�� */
                    if (sym == semicolon) /* �����ֺŽ������峣�� */
                    {
                        getsym();
                    }
                    else
                    {
                        error(6);   /* ©���˷ֺ� */
                    }
                }
                else if (sym == intsym)		/* ���������������ţ���ʼ����������� */
                {
                    getsym();
                    intdeclaration(&tx, lev, &dx);
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* ©���˷ֺ� */
                    }
                }
                else if (sym == boolsym)		/* ���������������ţ���ʼ����������� */
                {
                    getsym();
                    booldeclaration(&tx, lev, &dx);
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* ©���˷ֺ� */
                    }
                }
            }
            memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
            nxtlev[semicolon] = true;
            nxtlev[rbrace] = true;
            test(nxtlev, declbegsys, 7);
        } while (inset(sym, declbegsys));	/* ֱ��û���������� */

        cx0 = cx;
        gen(ini, 0, dx - dx0);	            /* ����ָ���ָ��ִ��ʱ������ջ��Ϊ�����õĹ��̿���dx����Ԫ�������� */

        while (inset(sym, statbegsys) || sym == semicolon)
        {
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);	/* ÿ����̷��ż��϶������ϲ��̷��ż��ϣ��Ա㲹�� */
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

        memset(nxtlev, 0, sizeof(bool) * symnum);	/* �ֳ���û�в��ȼ��� */
        test(fsys, nxtlev, 8);
    }

    if (tx0 == 0 && err == 0)
    {
        /* ������ű� */
        printf("\n===���ű�===\n");
        fprintf(ftable,"\n===���ű�===\n");
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
 * �ڷ��ű��м���һ��
 *
 * k:      ��ʶ��������Ϊconst��var��procedure
 * ptx:    ���ű�βָ���ָ�룬Ϊ�˿��Ըı���ű�βָ���ֵ
 * lev:    ��ʶ�����ڵĲ��
 * pdx:    dxΪ��ǰӦ����ı�������Ե�ַ�������Ҫ����1
 *
 */
void enter(bool bconst, enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	table[(*ptx)].bconst = bconst;
	strcpy(table[(*ptx)].name, id); /* ���ű��name���¼��ʶ�������� */
	table[(*ptx)].kind = k;
	switch (k)
	{
		case integer:
			if (bconst)     /* ���� */
			{
                if (num > amax)
                {
                    error(18);	/* ����Խ�� */
                    num = 0;
                }
                table[(*ptx)].val = num; /* �Ǽǳ�����ֵ */
			}
			else        /* ���� */
            {
                table[(*ptx)].level = lev;
                table[(*ptx)].adr = (*pdx);
                (*pdx)++;
            }
			break;
		case booltype:
			if (bconst)     /* ���� */
			{
                if (num != true && num != false)
                {
                    error(3);	/* ����ֵ�Ƿ� */
                    num = 0;
                }
                table[(*ptx)].val = num; /* �Ǽǲ���ֵ */
			}
			else        /* ���� */
            {
                table[(*ptx)].level = lev;
                table[(*ptx)].adr = (*pdx);
                (*pdx)++;
            }
			break;
	}
}

/*
 * ���ұ�ʶ���ڷ��ű��е�λ�ã���tx��ʼ������ұ�ʶ��
 * �ҵ��򷵻��ڷ��ű��е�λ�ã����򷵻�0
 *
 * id:    Ҫ���ҵ�����
 * tx:    ��ǰ���ű�βָ��
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
 * ������������
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
                    error(1);	/* ��=д����== */
                }
                getsym();
                if (sym == number)
                {
                    enter(true, integer, ptx, lev, pdx);
                    getsym();
                }
                else
                {
                    error(2);	/* ���������е�=��Ӧ������ */
                }
            }
            else
            {
                error(4);	/* ���������еı�ʶ����Ӧ��= */
            }
        }
        else
        {
            error(5);	/* ��������Ӧ�Ǳ�ʶ�� */
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
                    error(1);	/* Ҫ�á�=�������ǡ�==�� */
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
                    error(3);	/* ���������е�=��Ӧ��ture����false */
                }
            }
            else
            {
                error(4);	/* ���������еı�ʶ����Ӧ��= */
            }
        }
        else
        {
            error(5);	/* ��������Ӧ�Ǳ�ʶ�� */
        }
	}
	else
    {
        error(22);
    }
}

/*
 * ���ͱ�����������
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
		error(5);	/* int����Ӧ�Ǳ�ʶ�� */
    }
}

/*
 * ����������������
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
		error(5);	/* bool����Ӧ�Ǳ�ʶ�� */
    }
}

/*
 * �������Ŀ�����
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
 * ��䴦��
 */
void statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2, cx3, cx4, cx5, cx6, cx7;
	bool nxtlev[symnum];

	if (sym == ident)	/* ׼�����ո�ֵ��䴦�� */
	{
		i = position(id, *ptx);/* ���ұ�ʶ���ڷ��ű��е�λ�� */
		if (i == 0)
		{
			error(9);	/* ��ʶ��δ���� */
			getsym();
		}
		else
		{
			if(table[i].bconst)
            {
				error(10);	/* ��ֵ����У���ֵ���󲿱�ʶ��Ӧ���Ǳ��� */
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
                        expression(nxtlev, ptx, lev);	/* ����ֵ�����Ҳ���ʽ */
                        if(i != 0)
                        {
                            /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
                            gen(sto, lev-table[i].level, table[i].adr);
                        }
                    }
                    else
                    {
                        error(13);	/* û�м�⵽��ֵ���� */
                    }
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else if (sym != rparen)
                    {
                        error(6); /* ©���˷ֺ� */
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
                    expression(nxtlev, ptx, lev);	/* ����ֵ�����Ҳ���ʽ */
                    if(i != 0)
                    {
                        /* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
                        gen(sto, lev-table[i].level, table[i].adr);
                    }
                    if (sym == semicolon)
                    {
                        getsym();
                    }
                    else
                    {
                        error(6); /* ©���˷ֺ� */
                    }
                }
            }
        }
	}
	else
	{
		if (sym == readsym)	/* ׼������read��䴦�� */
		{
			getsym();
            if (sym == ident)
            {
                i = position(id, *ptx);	/* ����Ҫ���ı��� */
            }
            else
            {
                i = 0;
            }

            if (i == 0)
            {
                error(9);	/* read��������еı�ʶ��Ӧ�����������ı��� */
            }
            else
            {
                gen(opr, 0, 16);	/* ��������ָ���ȡֵ��ջ�� */
                gen(sto, lev-table[i].level, table[i].adr);	/* ��ջ���������������Ԫ�� */
            }
            getsym();
            if (sym == semicolon)
            {
                getsym();
            }
            else
            {
                error(6); /* ©���˷ֺ� */
            }
		}
		else
		{
			if (sym == writesym)	/* ׼������write��䴦�� */
			{
                getsym();
                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                gen(opr, 0, 14);	/* �������ָ����ջ����ֵ */
                gen(opr, 0, 15);	/* ���ɻ���ָ�� */
                if (sym == semicolon)
                {
                    getsym();
                }
                else
                {
                    error(6); /* ©���˷ֺ� */
                }
			}
			else
			{
                if (sym == ifsym)	/* ׼������if��䴦�� */
                {
                    getsym();
                    if (sym != lparen)
                    {
                        error(16);	/* ��ʽ����Ӧ�������� */
                    }
                    else
                    {
                        getsym();
                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        nxtlev[rparen] = true;	/* ��̷���Ϊ{ */
                        if(rel == true)
                        {
                            condition(nxtlev, ptx, lev); /* ������������ */
                            rel = false;
                        }
                        else
                        {
                            expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                        }
                    }
                    if(sym != rparen)
                    {
                        error(15);	/* ��ʽ����Ӧ�������� */
                        while (!inset(sym, fsys))	/* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
                        {
                            getsym();
                        }
                    }
                    else
                    {
                        getsym();
                    }

                    cx1 = cx;	/* ���浱ǰָ���ַ */
                    gen(jpc, 0, 0);	/* ����������תָ���ת��ַδ֪����ʱд0 */
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    nxtlev[elsesym] = true;	/* ��̷���Ϊ{ */
                    statement(nxtlev, ptx, lev);	/* ����then������ */
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
                    if (sym == lbrace)	/* ׼�����ո�����䴦�� */
                    {
                        block(lev, *ptx, nxtlev); /* �ݹ���� */
                    }
                    else
                    {
                        if (sym == whilesym)	/* ׼������while��䴦�� */
                        {
                            cx1 = cx;	/* �����ж�����������λ�� */
                            getsym();

                            if (sym != lparen)
                            {
                                error(16);	/* ��ʽ����Ӧ�������� */
                            }
                            else
                            {
                                getsym();
                                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                if(rel == true)
                                {
                                    condition(nxtlev, ptx, lev); /* ������������ */
                                    rel = false;
                                }
                                else
                                {
                                    expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                                }
                                cx2 = cx;	/* ����ѭ����Ľ�������һ��λ�� */
                                gen(jpc, 0, 0);	/* ����������ת��������ѭ���ĵ�ַδ֪�����Ϊ0�ȴ����� */
                            }
                            if(sym != rparen)
                            {
                                error(15);	/* ��ʽ����Ӧ�������� */
                                while (!inset(sym, fsys))	/* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
                                {
                                    getsym();
                                }
                            }
                            else
                            {
                                getsym();
                            }

                            statement(fsys, ptx, lev);	/* ѭ���� */
                            gen(jmp, 0, cx1);	/* ����������תָ���ת��ǰ���ж�����������λ�� */
                            code[cx2].a = cx;	/* ��������ѭ���ĵ�ַ */
                        }
                        else
                        {
                            if (sym == forsym)
                            {
                                getsym();
                                if (sym != lparen)
                                {
                                    error(16);	/* ��ʽ����Ӧ�������� */
                                }
                                else
                                {
                                    getsym();
                                    statement(fsys, ptx, lev);
                                    cx1 = cx;	/* �����ж�����������λ�� */
                                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                    if(rel == true)
                                    {
                                        condition(nxtlev, ptx, lev); /* ������������ */
                                        rel = false;
                                    }
                                    else
                                    {
                                        expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                                    }
                                    cx2 = cx;	/* ����ѭ����Ľ�������һ��λ�� */
                                    gen(jpc, 0, 0);	/* ����������ת��������ѭ���ĵ�ַδ֪�����Ϊ0�ȴ����� */
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
                                    error(15);	/* ��ʽ����Ӧ�������� */
                                    while (!inset(sym, fsys))	/* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
                                    {
                                        getsym();
                                    }
                                }
                                else
                                {
                                    getsym();
                                }
                                cx6 = cx;
                                statement(fsys, ptx, lev);	/* ѭ���� */
                                cx7 = cx;
                                gen(jmp, 0, 0);
                                gen(jmp, 0, cx1);	/* ����������תָ���ת��ǰ���ж�����������λ�� */
                                code[cx3].a = cx6;
                                code[cx7].a = cx4;
                                code[cx5].a = cx1;
                                code[cx2].a = cx;	/* ��������ѭ���ĵ�ַ */
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
                                        block(lev, *ptx, nxtlev); /* �ݹ���� */
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
                                            error(16);	/* ��ʽ����Ӧ�������� */
                                        }
                                        else
                                        {
                                            getsym();
                                            cx2 = cx;
                                            if(rel == true)
                                            {
                                                condition(nxtlev, ptx, lev); /* ������������ */
                                                rel = false;
                                            }
                                            else
                                            {
                                                expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                                            }
                                            cx3 = cx;
                                            gen(jeq, 0, 0);	/* ����������ת��������ѭ���ĵ�ַδ֪�����Ϊ0�ȴ����� */
                                            code[cx3].a = cx1;
                                        }
                                        if(sym != rparen)
                                        {
                                            error(15);	/* ��ʽ����Ӧ�������� */
                                            while (!inset(sym, fsys))	/* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
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
                                        block(lev, *ptx, nxtlev); /* �ݹ���� */
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
                                            error(16);	/* ��ʽ����Ӧ�������� */
                                        }
                                        else
                                        {
                                            getsym();
                                            cx2 = cx;
                                            if(rel == true)
                                            {
                                                condition(nxtlev, ptx, lev); /* ������������ */
                                                rel = false;
                                            }
                                            else
                                            {
                                                expression(nxtlev, ptx, lev);	/* ���ñ��ʽ���� */
                                            }
                                            cx3 = cx;
                                            gen(jpc, 0, 0);	/* ����������ת��������ѭ���ĵ�ַδ֪�����Ϊ0�ȴ����� */
                                            code[cx3].a = cx1;
                                        }
                                        if(sym != rparen)
                                        {
                                            error(15);	/* ��ʽ����Ӧ�������� */
                                            while (!inset(sym, fsys))	/* �����ȣ�ֱ�������ϲ㺯���ĺ�̷��� */
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
	memset(nxtlev, 0, sizeof(bool) * symnum);	/* �������޲��ȼ��� */
	test(fsys, nxtlev, 19);	/* �������������ȷ�� */
}

/*
 * ���ʽ����
 */
void expression(bool* fsys, int* ptx, int lev)
{
	enum symbol addop;	/* ���ڱ��������� */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[plus] = true;
	nxtlev[minus] = true;
	term(nxtlev, ptx, lev);	/* ������ */
	while (sym == plus || sym == minus || sym == orsym || sym == xorsym)
	{
		addop = sym;
		getsym();
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		term(nxtlev, ptx, lev);	/* ������ */
		if (addop == plus)
		{
			gen(opr, 0, 2);	/* ���ɼӷ�ָ�� */
		}
		else if (addop == minus)
		{
			gen(opr, 0, 3);	/* ���ɼ���ָ�� */
		}
		else if (addop == orsym)
		{
			gen(opr, 0, 18);	/* �����߼�������ָ�� */
		}
		else if (addop == xorsym)
		{
			gen(opr, 0, 20);	/* �����������ָ�� */
		}
	}
}

/*
 * ���
 */
void term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop;	/* ���ڱ���˳������� */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	nxtlev[mod] = true;
	nxtlev[xorsym] = true;
	factor(nxtlev, ptx, lev);	/* �������� */
	while(sym == times || sym == slash || sym == mod || sym == andsym)
	{
		mulop = sym;
		getsym();
		factor(nxtlev, ptx, lev);
		if(mulop == times)
		{
			gen(opr, 0, 4);	/* ���ɳ˷�ָ�� */
		}
		else if(mulop == slash)
		{
			gen(opr, 0, 5);	/* ���ɳ���ָ�� */
		}
		else if(mulop == mod)
        {
            gen(opr, 0, 7); /* ��������ָ��*/
        }
        else if(mulop == andsym)
        {
            gen(opr, 0, 17); /* �����߼�������ָ�� */
        }
	}
}

/*
 * ���Ӵ���
 */
void factor(bool* fsys, int* ptx, int lev)
{
	int i;
	bool nxtlev[symnum];
	//test(facbegsys, fsys, 24);	/* ������ӵĿ�ʼ���� */

	if (sym == not)
    {
        getsym();
        factor(nxtlev, ptx, lev);
        gen(opr, 0, 19);
    }
	while(inset(sym, facbegsys)) 	/* ѭ���������� */
	{
		if(sym == ident)	/* ����Ϊ��������� */
		{
			i = position(id, *ptx);	/* ���ұ�ʶ���ڷ��ű��е�λ�� */
			if (i == 0)
			{
				error(9);	/* ��ʶ��δ���� */
				getsym();
			}
			else
			{
                if (table[i].bconst)	/* ��ʶ��Ϊ���� */
                {
                    gen(lit, 0, table[i].val);	/* ֱ�Ӱѳ�����ֵ��ջ */
                }
                else
                {
                    gen(lod, lev-table[i].level, table[i].adr);	/* �ҵ�������ַ������ֵ��ջ */
                }
                getsym();
			}
		}
		else
		{
			if(sym == number)	/* ����Ϊ�� */
			{
				if (num > amax)
				{
					error(18); /* ��Խ�� */
					num = 0;
				}
				gen(lit, 0, num);
				getsym();
			}
			else
			{
				if (sym == lparen)	/* ����Ϊ���ʽ */
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
						error(16);	/* ȱ�������� */
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
		//test(fsys, nxtlev, 23); /* һ�����Ӵ�����ϣ������ĵ���Ӧ��fsys������ */
		                        /* ������ǣ������ҵ���һ�����ӵĿ�ʼ��ʹ�﷨�������Լ���������ȥ */
	}
}

/*
 * ��������
 */
void condition(bool* fsys, int* ptx, int lev)
{
	int i;
	enum symbol relop;
	bool nxtlev[symnum];

	if(sym == oddsym)	/* ׼������odd���㴦�� */
	{
		getsym();
		if (sym == lparen)	/* ����Ϊ���ʽ */
        {
            getsym();
            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[rparen] = true;
            expression(nxtlev, ptx, lev);
            gen(opr, 0, 6);	/* ����oddָ�� */
            if (sym == rparen)
            {
                getsym();
            }
            else
            {
                error(15);	/* ȱ�������� */
            }
        }
	}
	else
	{
		/* �߼����ʽ���� */
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
			error(14); /* Ӧ��Ϊ��ϵ����� */
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
 * ���ͳ���
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

	int p = 0; /* ָ��ָ�� */
	int b = 1; /* ָ���ַ */
	int t = 0; /* ջ��ָ�� */
	struct instruction i;	/* ��ŵ�ǰָ�� */
	int s[stacksize];	/* ջ */

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

	printf("\n===��ʼ����...===\n");
	fprintf(fresult,"\n===��ʼ����...===\n");
	s[0] = 0; /* s[0]���� */
	s[1] = 0; /* �������������ϵ��Ԫ����Ϊ0 */
	s[2] = 0;
	s[3] = 0;
	do {
	    i = code[p];	/* ����ǰָ�� */
		p = p + 1;
		switch (i.f)
		{
			case lit:	/* ������a��ֵȡ��ջ�� */
				t = t + 1;
				s[t] = i.a;
				break;
			case opr:	/* ��ѧ���߼����� */
				switch (i.a)
				{
					case 0:  /* �������ý����󷵻� */
						t = b - 1;
						p = s[t + 3];
						b = s[t + 2];
						break;
					case 1: /* ջ��Ԫ��ȡ�� */
						s[t] = - s[t];
						break;
					case 2: /* ��ջ�������ջ���������ջԪ�أ����ֵ��ջ */
						t = t - 1;
						s[t] = s[t] + s[t + 1];
						break;
					case 3:/* ��ջ�����ȥջ���� */
						t = t - 1;
						s[t] = s[t] - s[t + 1];
						break;
					case 4:/* ��ջ�������ջ���� */
						t = t - 1;
						s[t] = s[t] * s[t + 1];
						break;
					case 5:/* ��ջ�������ջ���� */
						t = t - 1;
						s[t] = s[t] / s[t + 1];
						break;
					case 6:/* ջ��Ԫ�ص���ż�ж� */
						s[t] = s[t] % 2;
						break;
                    case 7:/* ��ջ����ģ��ջ���� */
						t = t - 1;
						s[t] = s[t] % s[t + 1];
						break;
					case 8:/* ��ջ������ջ�����Ƿ���� */
						t = t - 1;
						s[t] = (s[t] == s[t + 1]);
						break;
					case 9:/* ��ջ������ջ�����Ƿ񲻵� */
						t = t - 1;
						s[t] = (s[t] != s[t + 1]);
						break;
					case 10:/* ��ջ�����Ƿ�С��ջ���� */
						t = t - 1;
						s[t] = (s[t] < s[t + 1]);
						break;
					case 11:/* ��ջ�����Ƿ���ڵ���ջ���� */
						t = t - 1;
						s[t] = (s[t] >= s[t + 1]);
						break;
					case 12:/* ��ջ�����Ƿ����ջ���� */
						t = t - 1;
						s[t] = (s[t] > s[t + 1]);
						break;
					case 13: /* ��ջ�����Ƿ�С�ڵ���ջ���� */
						t = t - 1;
						s[t] = (s[t] <= s[t + 1]);
						break;
					case 14:/* ջ��ֵ��� */
						printf("%d", s[t]);
						fprintf(fresult, "%d", s[t]);
						t = t - 1;
						break;
					case 15:/* ������з� */
						printf("\n");
						fprintf(fresult,"\n");
						break;
					case 16:/* ����һ����������ջ�� */
						t = t + 1;
						printf("?");
						fprintf(fresult, "?");
						scanf("%d", &(s[t]));
						fprintf(fresult, "%d\n", s[t]);
						break;
                    case 17:/* ��ջ������ջ��������߼������� */
                        t = t - 1;
						s[t] = (s[t] & s[t + 1]);
						break;
                    case 18:/* ��ջ������ջ��������߼������� */
                        t = t - 1;
						s[t] = (s[t] | s[t + 1]);
						break;
                    case 19:/* ջ��Ԫ���߼�ȡ�� */
						s[t] = s[t] ^ 1;
						break;
                    case 20:/* ��ջ������ջ�������������� */
                        t = t - 1;
						s[t] = (s[t] ^ s[t + 1]);
						break;
				}
				break;
			case lod:	/* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
				t = t + 1;
				s[t] = s[base(i.l,s,b) + i.a];
				break;
			case sto:	/* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
				s[base(i.l, s, b) + i.a] = s[t];
				t = t - 1;
				break;
			case ini:	/* ������ջ��Ϊ�����õĹ��̿���a����Ԫ�������� */
				t = t + i.a;
				break;
			case jmp:	/* ֱ����ת */
				p = i.a;
				break;
			case jpc:	/* ������ת */
				if (s[t] == 0)
					p = i.a;
				t = t - 1;
				break;
            case jeq:	/* ������ת */
				if (s[t] != 0)
					p = i.a;
				t = t - 1;
				break;
		}
	} while (p != 0);
	printf("\n===���н���===\n\n");
	fprintf(fresult,"\n===���н���===\n");
    fclose(fresult);
    fclose(fin);
}

/* ͨ�����̻�ַ����l����̵Ļ�ַ */
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
 * ���Գ���
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

	int p = 0; /* ָ��ָ�� */
	int b = 1; /* ָ���ַ */
	int t = 0; /* ջ��ָ�� */
	struct instruction i;	/* ��ŵ�ǰָ�� */
	int s[stacksize];	/* ջ */

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

	printf("\n===��ʼ����...===\n");
	s[0] = 0; /* s[0]���� */
	s[1] = 0; /* �������������ϵ��Ԫ����Ϊ0 */
	s[2] = 0;
	s[3] = 0;
	do {
	    i = code[p];	/* ����ǰָ�� */
		p = p + 1;
		switch (i.f)
		{
			case lit:	/* ������a��ֵȡ��ջ�� */
				t = t + 1;
				s[t] = i.a;
				break;
			case opr:	/* ��ѧ���߼����� */
				switch (i.a)
				{
					case 0:  /* �������ý����󷵻� */
						t = b - 1;
						p = s[t + 3];
						b = s[t + 2];
						break;
					case 1: /* ջ��Ԫ��ȡ�� */
						s[t] = - s[t];
						break;
					case 2: /* ��ջ�������ջ���������ջԪ�أ����ֵ��ջ */
						t = t - 1;
						s[t] = s[t] + s[t + 1];
						break;
					case 3:/* ��ջ�����ȥջ���� */
						t = t - 1;
						s[t] = s[t] - s[t + 1];
						break;
					case 4:/* ��ջ�������ջ���� */
						t = t - 1;
						s[t] = s[t] * s[t + 1];
						break;
					case 5:/* ��ջ�������ջ���� */
						t = t - 1;
						s[t] = s[t] / s[t + 1];
						break;
					case 6:/* ջ��Ԫ�ص���ż�ж� */
						s[t] = s[t] % 2;
						break;
                    case 7:/* ��ջ����ģ��ջ���� */
						t = t - 1;
						s[t] = s[t] % s[t + 1];
						break;
					case 8:/* ��ջ������ջ�����Ƿ���� */
						t = t - 1;
						s[t] = (s[t] == s[t + 1]);
						break;
					case 9:/* ��ջ������ջ�����Ƿ񲻵� */
						t = t - 1;
						s[t] = (s[t] != s[t + 1]);
						break;
					case 10:/* ��ջ�����Ƿ�С��ջ���� */
						t = t - 1;
						s[t] = (s[t] < s[t + 1]);
						break;
					case 11:/* ��ջ�����Ƿ���ڵ���ջ���� */
						t = t - 1;
						s[t] = (s[t] >= s[t + 1]);
						break;
					case 12:/* ��ջ�����Ƿ����ջ���� */
						t = t - 1;
						s[t] = (s[t] > s[t + 1]);
						break;
					case 13: /* ��ջ�����Ƿ�С�ڵ���ջ���� */
						t = t - 1;
						s[t] = (s[t] <= s[t + 1]);
						break;
					case 14:/* ջ��ֵ��� */
						printf("%d", s[t]);
						fprintf(fresult, "%d", s[t]);
						t = t - 1;
						break;
					case 15:/* ������з� */
						printf("\n");
						fprintf(fresult,"\n");
						break;
					case 16:/* ����һ����������ջ�� */
						t = t + 1;
						printf("?");
						fprintf(fresult, "?");
						scanf("%d", &(s[t]));
						fprintf(fresult, "%d\n", s[t]);
						break;
                    case 17:/* ��ջ������ջ��������߼������� */
                        t = t - 1;
						s[t] = (s[t] & s[t + 1]);
						break;
                    case 18:/* ��ջ������ջ��������߼������� */
                        t = t - 1;
						s[t] = (s[t] | s[t + 1]);
						break;
                    case 19:/* ջ��Ԫ���߼�ȡ�� */
						s[t] = s[t] ^ 1;
						break;
                    case 20:/* ��ջ������ջ�������������� */
                        t = t - 1;
						s[t] = (s[t] ^ s[t + 1]);
						break;
				}
				break;
			case lod:	/* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
				t = t + 1;
				s[t] = s[base(i.l,s,b) + i.a];
				break;
			case sto:	/* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
				s[base(i.l, s, b) + i.a] = s[t];
				t = t - 1;
				break;
			case ini:	/* ������ջ��Ϊ�����õĹ��̿���a����Ԫ�������� */
				t = t + i.a;
				break;
			case jmp:	/* ֱ����ת */
				p = i.a;
				break;
			case jpc:	/* ������ת */
				if (s[t] == 0)
					p = i.a;
				t = t - 1;
				break;
            case jeq:	/* ������ת */
				if (s[t] != 0)
					p = i.a;
				t = t - 1;
				break;
		}
		printf("��ǰָ�%s %d %d\n", mnemonic[i.f], i.l, i.a);
		printf("===����ջ===\n");
		int j;
		for(j = t; j >= 0; j--)
            printf("%d\t%d\n", j, s[j]);
		printf("���س���������һ��ָ��...\n");
		while(getchar() == '\r')
            break;
	} while (p != 0);
	printf("\n===���н���===\n\n");
    fclose(fin);
}
