/* File: japanese.c */


#include "angband.h"

#ifdef JP

/*���ܸ�ư����� (�Ǥġ��Ǥä�,�Ǥ� etc) */

#define CMPTAIL(y) strncmp(&in[l-strlen(y)],y,strlen(y))

/* ����,����䲥��,���� */
void jverb1( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"��");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"��");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"��");else

  sprintf(&out[l],"������");}

/* ����,����> ���äƽ��� */
void jverb2( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"�Ƥä�");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"�Ǥ�");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"�ͤ�");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"�ؤ�");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"�٤�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"���");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"���");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ͤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ؤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
  sprintf(&out[l],"���Ȥˤ��");}

/* ����,���� > ���ä��꽳�ä��� */
void jverb3( const char *in , char *out){
int l=strlen(in);
strcpy(out,in);

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else

if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("����")==0) sprintf(&out[l-4],"����");else
if( CMPTAIL("�Ƥ�")==0) sprintf(&out[l-4],"�Ƥä�");else
if( CMPTAIL("�Ǥ�")==0) sprintf(&out[l-4],"�Ǥ�");else
if( CMPTAIL("�ͤ�")==0) sprintf(&out[l-4],"�ͤ�");else
if( CMPTAIL("�ؤ�")==0) sprintf(&out[l-4],"�ؤ�");else
if( CMPTAIL("�٤�")==0) sprintf(&out[l-4],"�٤�");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"�᤿");else
if( CMPTAIL("���")==0) sprintf(&out[l-4],"�줿");else

if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"����");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ͤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ؤ�");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"���");else
if( CMPTAIL("��")==0) sprintf(&out[l-2],"�ä�");else
  sprintf(&out[l],"���Ȥ�");}


void jverb( const char *in , char *out , int flag){
  switch (flag){
  case JVERB_AND:jverb1(in , out);break;
  case JVERB_TO :jverb2(in , out);break;
  case JVERB_OR :jverb3(in , out);break;
  }
}

char* strstr_j(const char* s, const char* t)
{
	int i, l1, l2;

	l1 = strlen(s);
	l2 = strlen(t);
	if (l1 >= l2) {
		for(i = 0; i <= l1 - l2; i++) {
			if(!strncmp(s + i, t, l2))
				return (char *)s + i;
			if (iskanji(*(s + i)))
				i++;
		}
	}
	return NULL;
}


char *strchr_j(const char *ptr, char ch)
{
	for ( ; *ptr != '\0'; ++ptr)
	{
		if (*ptr == ch) return (char *)ptr;
		if (iskanji(*ptr)) ++ptr;
	}

	return NULL;
}


/*
 * Convert SJIS string to EUC string
 */
void sjis2euc(char *str)
{
	int i;
	unsigned char c1, c2;
	unsigned char *tmp;

	int len = strlen(str);

	C_MAKE(tmp, len+1, byte);

	for (i = 0; i < len; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c2 >= 0x9f)
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe0 : 0x60);
				c2 += 2;
			}
			else
			{
				c1 = c1 * 2 - (c1 >= 0xe0 ? 0xe1 : 0x61);
				c2 += 0x60 + (c2 < 0x7f);
			}
			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[len] = 0;
	strcpy(str, (char *)tmp);

	C_KILL(tmp, len+1, byte);
}  


/*
 * Convert EUC string to SJIS string
 */
void euc2sjis(char *str)
{
	int i;
	unsigned char c1, c2;
	unsigned char *tmp;
	
	int len = strlen(str);

	C_MAKE(tmp, len+1, byte);

	for (i = 0; i < len; i++)
	{
		c1 = str[i];
		if (c1 & 0x80)
		{
			i++;
			c2 = str[i];
			if (c1 % 2)
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x31 : 0x71);
				c2 -= 0x60 + (c2 < 0xe0);
			}
			else
			{
				c1 = (c1 >> 1) + (c1 < 0xdf ? 0x30 : 0x70);
				c2 -= 2;
			}

			tmp[i - 1] = c1;
			tmp[i] = c2;
		}
		else
			tmp[i] = c1;
	}
	tmp[len] = 0;
	strcpy(str, (char *)tmp);

	C_KILL(tmp, len+1, byte);
}  


/*
 * str��Ķ��˹�ä�ʸ�������ɤ��Ѵ������Ѵ�����ʸ�������ɤ��֤���
 * str��Ĺ�������¤Ϥʤ���
 *
 * 0: Unknown
 * 1: ASCII (Never known to be ASCII in this function.)
 * 2: EUC
 * 3: SJIS
 */
byte codeconv(char *str)
{
	byte code = 0;
	int i;

	for (i = 0; str[i]; i++)
	{
		unsigned char c1;
		unsigned char c2;

		/* First byte */
		c1 = str[i];

		/* ASCII? */
		if (!(c1 & 0x80)) continue;

		/* Second byte */
		i++;
		c2 = str[i];

		if (((0xa1 <= c1 && c1 <= 0xdf) || (0xfd <= c1 && c1 <= 0xfe)) &&
		    (0xa1 <= c2 && c2 <= 0xfe))
		{
			/* Only EUC is allowed */
			if (!code)
			{
				/* EUC */
				code = 2;
			}

			/* Broken string? */
			else if (code != 2)
			{
				/* No conversion */
				return 0;
			}
		}

		else if (((0x81 <= c1 && c1 <= 0x9f) &&
			  ((0x40 <= c2 && c2 <= 0x7e) || (0x80 <= c2 && c2 <= 0xfc))) ||
			 ((0xe0 <= c1 && c1 <= 0xfc) &&
			  (0x40 <= c2 && c2 <= 0x7e)))
		{
			/* Only SJIS is allowed */
			if (!code)
			{
				/* SJIS */
				code = 3;
			}

			/* Broken string? */
			else if (code != 3)
			{
				/* No conversion */
				return 0;
			}
		}
	}


	switch (code)
	{
#ifdef EUC
	case 3:
		/* SJIS -> EUC */
		sjis2euc(str);
		break;
#endif

#ifdef SJIS
	case 2:
		/* EUC -> SJIS */
		euc2sjis(str);

		break;
#endif
	}

	/* Return kanji code */
	return code;
}

/* ʸ����s��x�Х����ܤ�������1�Х����ܤ��ɤ���Ƚ�ꤹ�� */
bool iskanji2(cptr s, int x)
{
	int i;

	for (i = 0; i < x; i++)
	{
		if (iskanji(s[i])) i++;
	}
	if ((x == i) && iskanji(s[x])) return TRUE;

	return FALSE;
}

#endif /* JP */

