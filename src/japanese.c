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

/* 2�Х���ʸ�����θ���ʤ������ n �Х���ʸ����򥳥ԡ����� */
size_t mb_strlcpy(char *dst, const char *src, size_t size)
{
	unsigned char *d = (unsigned char*)dst;
	const unsigned char *s = (unsigned char*)src;
	size_t n = 0;

	/* reserve for NUL termination */
	size--;

	/* Copy as many bytes as will fit */
	while(n < size) {
		if (iskanji(*d)) {
			if(n + 2 >= size || !*(d+1)) break;
			*d++ = *s++;
			*d++ = *s++;
			n += 2;
		} else {
			*d++ = *s++;
			n++;
		}
	}
	*d = '\0';
	while(*s++) n++;
	return n;
}

#endif /* JP */

