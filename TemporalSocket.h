#ifndef _SOCKETDEF
#define _SOCKETDEF

#ifndef __WINSOCKDEF
#include <winsock2.h>
#endif

#include <stdio.h>

#include "Temporal.h"

#pragma comment(lib, "ws2_32.lib")

class TemporalSocket
{
public:
	TemporalSocket(Temporal* pTemp);
	~TemporalSocket(void);

	//The last Exception message sent by the connection
	char LastExeceptionMsg[200];

	//All the currently Connected clients (up to 16 clients with names up to 32 characters ling
	char* AllConnClients[16][32];

	//The currently Connected Socket
	int CurrentSocket;

	//Connect to a server from the client
	bool ClientConnect(char* IPAddress, int HostPort);

	//Send message to Server from client
	char* ClientSend(char* Message);

	//Close the connection to the server
	bool ClientDisconnect(int hsock);

	//Host a server
	bool ServerHost(int HostPort);

	DWORD WINAPI SocketHandler(VOID* lpParameter);

	DWORD WINAPI AcceptHandler(VOID* lpParameter);

	Temporal* ParentTemporal;
};

DWORD __stdcall startMethodInThread( LPVOID arg );
DWORD __stdcall startServerInThread( LPVOID arg );

#endif

