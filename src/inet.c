/* File: inet.c */

#include "angband.h"

#ifdef WORLD_SCORE

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#if defined(WINDOWS)
#include <winsock.h>
#elif defined(MACINTOSH)
#include <OpenTransport.h>
#include <OpenTptInternet.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#include <setjmp.h>
#include <signal.h>
#endif

#include <stdlib.h>

static char	*errstr;
static char	*proxy;
static int	proxy_port;

#ifdef MACINTOSH
static InetSvcRef inet_services = nil;
static EndpointRef ep 		= kOTInvalidEndpointRef;
#endif

#if 0 /* �Ȥꤢ�������ߤϻȤ�ʤ���by Habu*/
static char	*homeurl;

void
set_homeurl(char *s)
{
	if(homeurl)
		free(homeurl);

	homeurl = malloc(strlen(s) + 1);
	strcpy(homeurl, s);
}

char *
get_homeurl()
{
	if(homeurl)
		return homeurl;
	else
		return "";
}


char *
get_proxy()
{
	static char buf[BUFSIZ];

	if(proxy && proxy[0]){
#ifndef WINDOWS
		snprintf(buf, sizeof(buf), "%s:%d", proxy, proxy_port);
#else
		_snprintf(buf, sizeof(buf), "%s:%d", proxy, proxy_port);
#endif
		buf[sizeof(buf)-1] = '\0';
		return buf;
	}
	else
		return "";
}

char *
get_proxy_host()
{
	return proxy;
}

int
get_proxy_port()
{
	return proxy_port;
}

int soc_read(int sd, char *buf, size_t sz)
{
#ifndef WINDOWS
	return read(sd, buf, sz);
#else
	return recv(sd, buf, sz, 0);
#endif
}

#endif /* if 0 */

/* �ץ��������ФΥ��ɥ쥹���ե����뤫���ɤ�����ꤹ�� */
static void set_proxy()
{
	char buf[1024];
	size_t len;
	FILE *fp;
	char *s;

#ifdef MACINTOSH
	int i;
	char tmp[8];
#endif

	path_build(buf, 1024, ANGBAND_DIR_PREF, "proxy.prf");

	/* �ե����뤫��������ɤࡣ*/
	fp = my_fopen(buf, "r");

	if (!fp)
	{
		/* �ե����뤬¸�ߤ��ʤ�����define.h��Υǥե���Ȥ����� */
		proxy = HTTP_PROXY;
		proxy_port = HTTP_PROXY_PORT;
		return;
	}

	while (my_fgets(fp, buf, 1024)==0)
	{
		if (buf[0] != '#' && buf[0] != '\0') break;
	}

	my_fclose(fp);

	/* �ݥ��󥿤��Ѱա�*/
	s = buf;

	/* "http://" ����ϤޤäƤ�����Ϥ�����ʬ�򥫥åȤ��롣*/
#if defined(WINDOWS)
	if (!strnicmp(s, "http://", 7))
	{
		s += 7;
	}
#elif defined(MACINTOSH)
	strncpy( tmp , s , 7 );
	for ( i = 0 ; i < 7 ; i++ )
	{
		if ( isalpha(tmp[i]) )
			tmp[i]= tolower(tmp[i]);
	}
	if (!strncmp(s, "http://", 7))
	{
		s += 7;
	}
#else
	if (!strncasecmp(s, "http://", 7))
	{
		s += 7;
	}
#endif

	/* ʸ�����Ĺ����Ĵ�١�ɬ�פʥ������� */
	len = strlen(s);
	proxy = malloc(len + 1);

	/* �ݡ����ֹ椬���뤫�ɤ�����Ĵ�١������proxy_port�����ꡣ*/
	--len;
	while (len > 0 && isdigit(s[len]))
		--len;
	if (len > 0 && s[len] == ':' && s[len + 1] != '\0')
	{
		s[len] = '\0';
		strcpy(proxy, s);
		proxy_port = atoi(s + (len + 1));
	}
	else
	{
		strcpy(proxy, s);
		proxy_port = HTTP_PROXY_PORT;
	}

	/* �ץ����Υ��ɥ쥹��proxy�˥��ԡ� */
	strcpy(proxy, s);

	if (proxy_port == 0)
		proxy_port = 80;
}

/* �����åȤ˥Хåե������Ƥ�񤭹��� */
int soc_write(int sd, char *buf, size_t sz)
{
#ifndef MACINTOSH
#ifndef WINDOWS
	write(sd, buf, sz);
#else
	int nleft, nwritten;
	
	nleft = sz;

	while (nleft > 0) {
		nwritten = send(sd, buf, nleft, 0);
		if (nwritten <= 0)
			return (nwritten);
		nleft -= nwritten;
		buf += nwritten;
	}
#endif
#else /* !MACINTOSH */

	OTResult bytesSent;
	
	OTSnd(ep, (void *) buf, sz, 0);

#endif
	return sz;
}

#if 0 /* �����餯�Ȥ�ʤ� */
int soc_write_str(int sd, char *buf)
{
	return soc_write(sd, buf, strlen(buf));
}
#endif

#if !defined(WINDOWS) && !defined(MACINTOSH)
static jmp_buf	env;
static void (*sig_int_saved)(int);
static void (*sig_alm_saved)(int);
#endif

static void restore_signal()
{
#if !defined(WINDOWS) && !defined(MACINTOSH)
	struct itimerval	val0;

	/* itimer�ꥻ�å��� */
	val0.it_interval.tv_sec = 0;
	val0.it_interval.tv_usec = 0;
	val0.it_value.tv_sec = 0;
	val0.it_value.tv_usec = 0;

	/* ���顼���� */
	setitimer(ITIMER_REAL, &val0, NULL);
	signal(SIGALRM, sig_alm_saved);
	signal(SIGINT, sig_int_saved);
#endif
}


#if !defined(WINDOWS) && !defined(MACINTOSH)
static void interrupt_report(int sig)
{
	restore_signal();
	siglongjmp(env, sig);
}
#endif


#ifndef MACINTOSH
/* �����Ф˥��ͥ��Ȥ���ؿ��� Win, unix */
static int connect_server(int timeout, const char *host, int port)
{
	int			sd;
	struct sockaddr_in	to;
	struct hostent	*hp;

#ifndef WINDOWS
	struct itimerval	val;
	int			ret;

	/* itimer������ */
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 0;
	val.it_value.tv_sec = timeout;
	val.it_value.tv_usec = 0;

	/* �����ॢ���ȡ��⤷�������Ǥ������ν�����*/
	if ((ret = sigsetjmp(env,1)) != 0)
	{
#ifdef JP
		if (ret == SIGALRM)
			errstr = "���顼: �����ॢ����";
		else
			errstr = "���顼: ���󥿥�ץ�";
#else
		if (ret == SIGALRM)
			errstr = "Error : time out";
		else
			errstr = "Error : interupted";
#endif
		return -1;
	}
	sig_int_saved = signal(SIGINT, interrupt_report);
	sig_alm_saved = signal(SIGALRM, interrupt_report);
    
	/* �����ॢ���Ȥλ��֤����� */
	setitimer(ITIMER_REAL, &val, NULL);
#endif

	/* �ץ��������ꤵ��Ƥ���Хץ����˷Ҥ� */
	if (proxy && proxy[0])
	{
		if ((hp = gethostbyname(proxy)) == NULL)
		{
#ifdef JP
			errstr = "���顼: �ץ����Υ��ɥ쥹�������Ǥ�";
#else
			errstr = "Error : wrong proxy addres";
#endif

			restore_signal();

			return -1;
		}
	}
	else if ((hp = gethostbyname(host)) == NULL)
	{
#ifdef JP
		errstr = "���顼: �����ФΥ��ɥ쥹�������Ǥ�";
#else
		errstr = "Error : wrong server adress";
#endif

		restore_signal();

		return -1;
	}

	memset(&to, 0, sizeof(to));
	memcpy(&to.sin_addr, hp->h_addr_list[0], hp->h_length);

	to.sin_family = AF_INET;

	if(proxy && proxy[0] && proxy_port)
		to.sin_port = htons(proxy_port);
	else
		to.sin_port = htons(port);

#ifndef WINDOWS
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
#ifdef JP
		errstr = "���顼: �����åȤ������Ǥ��ޤ���";
#else
		errstr = "Error : cannot create socket.";
#endif
		restore_signal();
		return -1;
	}
#else
	if  ((sd = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
#ifdef JP
		errstr = "���顼: �����åȤ������Ǥ��ޤ���";
#else
		errstr = "Error : cannot create socket.";
#endif
		restore_signal();
		return -1;
	}
#endif

	if (connect(sd, (struct sockaddr *)&to, sizeof(to)) < 0)
	{
#ifdef JP
		errstr = "���顼: �����Ф���³�Ǥ��ޤ���";
#else
		errstr = "Error : failed to connect server";
#endif
		restore_signal();
#ifndef WINDOWS
		close(sd);
#else
		closesocket(sd);
#endif
		return -1;
	}

	restore_signal();

	return sd;
}

#else /* !MACINTOSH */

/* �����Ф˥��ͥ��Ȥ���ؿ��� Mac */
static int connect_server(int timeout, const char *host, int port)
{
	OSStatus err;
	InetHostInfo 	response;
	InetHost 		host_addr;
	InetAddress 	inAddr;
	TCall 			sndCall;
	Boolean			bind	= false;
	
	memset(&response, 0, sizeof(response));
	
#if TARGET_API_MAC_CARBON
	inet_services = OTOpenInternetServicesInContext(kDefaultInternetServicesPath, 0, &err, NULL);
#else
	inet_services = OTOpenInternetServices(kDefaultInternetServicesPath, 0, &err);
#endif 
	
	if (err == noErr) {
		
		if (proxy && proxy[0])
		{
			err = OTInetStringToAddress(inet_services, proxy, &response);
		}
		else
		{
			err = OTInetStringToAddress(inet_services, (char *)host, &response);
		}
		
		if (err == noErr)
		{
			host_addr = response.addrs[0];
		}
		else
		{
			errstr = "error: bad score server!\n";
		}
		
#if TARGET_API_MAC_CARBON
		ep = (void *)OTOpenEndpointInContext(OTCreateConfiguration(kTCPName), 0, nil, &err, NULL);
#else
		ep = (void *)OTOpenEndpoint(OTCreateConfiguration(kTCPName), 0, nil, &err);
#endif

		if (err == noErr)
		{
			err = OTBind(ep, nil, nil);
			bind = (err == noErr);
		}
		if (err == noErr)
		{
			if (proxy && proxy[0] && proxy_port)
				OTInitInetAddress(&inAddr, proxy_port, host_addr);
			else
				OTInitInetAddress(&inAddr, port, host_addr);
			
			sndCall.addr.len 	= sizeof(InetAddress);				
			sndCall.addr.buf	= (unsigned char*) &inAddr;
			sndCall.opt.buf 	= nil;	      /* no connection options */
			sndCall.opt.len 	= 0;
			sndCall.udata.buf 	= nil;	      /* no connection data */
			sndCall.udata.len 	= 0;
			sndCall.sequence 	= 0;	      /* ignored by OTConnect */
			
			err = OTConnect(ep, &sndCall, NULL);
			
			if (err != noErr)
			{
				errstr = "error: cannot connect score server!\n";
			}
		}
	}
	
	if ( err != noErr )
	{
		if ( bind )
		{
			OTUnbind(ep);
		}
		/* Clean up. */
		if (ep != kOTInvalidEndpointRef)
		{
			OTCloseProvider(ep);
			ep = nil;
		}
		if (inet_services != nil)
		{
			OTCloseProvider(inet_services);
			inet_services = nil;
		}
	
		return -1;
	}
	
	return 1;
}
#endif

int connect_scoreserver(void)
{
	/* �ץ��������ꤹ�� */
	set_proxy();

	return connect_server(HTTP_TIMEOUT, SCORE_SERVER, SCORE_PORT);
}

int disconnect_server(int sd)
{
#if defined(WINDOWS)
	return closesocket(sd);
#elif defined(MACINTOSH)
	if (ep != kOTInvalidEndpointRef)
	{
		OTCloseProvider(ep);
	}
	
	if (inet_services != nil)
	{
		OTCloseProvider(inet_services);
	}
#else
	return close(sd);
#endif
}

char *
soc_err()
{
	return errstr;
}

#endif /* WORLD_SCORE */
