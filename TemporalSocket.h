#ifndef _SOCKETDEF
#define _SOCKETDEF

#ifndef __WINSOCKDEF
#include <winsock2.h>
#endif

#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

class TemporalSocket
{
public:
	TemporalSocket(void);
	~TemporalSocket(void);

	char LastExeceptionMsg[];

	bool ServerConnect(char* IPAddress, int HostPort);

	bool ServerHost(int HostPort);

	DWORD WINAPI SocketHandler(VOID* lpParameter);
};

DWORD __stdcall startMethodInThread( LPVOID arg );

#endif

