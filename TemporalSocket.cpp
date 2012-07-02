#include "TemporalSocket.h"


TemporalSocket::TemporalSocket(Temporal* pTemp)
{
	memset(LastExeceptionMsg, '\0', 1024);

	for( int i=0; i<16; i++)
	{
		memset(AllConnClients[i], '\0', 32);
	}

	CurrentSocket = -1;

	ParentTemporal = pTemp;
}


TemporalSocket::~TemporalSocket(void)
{
}

//Connect to the server as a client
bool TemporalSocket::ClientConnect(char* IPAddress,  int HostPort)
{
	/*
	//The port and address you want to connect to
	int host_port= 1101;
	char* host_name="127.0.0.1";
	*/

	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	//The Windows Socket Version (2.2)
	wVersionRequested = MAKEWORD( 2, 2 );

	//Start up the WIndows Socket
	err = WSAStartup( wVersionRequested, &wsaData );

	//If something went wrong (err != 0) OR the version is not 2.2.  ERROR out
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )) 
	{
		sprintf(LastExeceptionMsg, "Could not find useable sock dll %d\n",WSAGetLastError());
		return false;
	}

	//Initialize sockets and set any options
	int hsock;
	int * p_int;
	hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(hsock == -1)
	{
		sprintf(LastExeceptionMsg, "Error initializing socket %d\n",WSAGetLastError());
		return false;
	}

	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;

	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 ) || (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) )
	{
		sprintf(LastExeceptionMsg, "Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return false;
	}

	free(p_int);

	//Connect to the server
	struct sockaddr_in my_addr;
	
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(HostPort);
	     
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(IPAddress);

	if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR )
	{
		sprintf(LastExeceptionMsg, "Error connecting socket %d\n", WSAGetLastError());
		return false;
	}

	CurrentSocket = hsock;

	return true;
}


//Send message to Server from client
char* TemporalSocket::ClientSend(char* Message)
{
	//The Message that needs to be sent
	char buffer[1024];

	//The Length of the message
	int buffer_len = 1024;

	//The bytecount of the message sent
	int bytecount;
	  
	
	//Set aside a 1024 block of char's as NULL/Blanks
	memset(buffer, '\0', buffer_len);

	strcpy(buffer, Message);
	
	//Send the Message (The Soket, the message, the length of the message
	bytecount=send(CurrentSocket, buffer, strlen(buffer),0);

	if( bytecount == SOCKET_ERROR)
	{
		sprintf(LastExeceptionMsg,  "Error sending data %d\n", WSAGetLastError());
		return false;
	}
	sprintf(LastExeceptionMsg, "Sent bytes %d\n", bytecount);
	
	//Wait for recieved signal
	bytecount = recv(CurrentSocket, buffer, buffer_len, 0);

	if(bytecount == SOCKET_ERROR)
	{
		sprintf(LastExeceptionMsg, "Error receiving data %d\n", WSAGetLastError());
		return false;
	}

	char* ReturnMessage = (char*)malloc(strlen(buffer)+1);
	memset(ReturnMessage, '\0', strlen(buffer)+1);
	strcpy(ReturnMessage, buffer);

	return ReturnMessage;

}

//Close the connection to the server
bool TemporalSocket::ClientDisconnect(int hsock)
{
	if(hsock != -1)
	{
		closesocket(hsock);
		return true;
	}
	return false;
}

//Create a server Host
bool TemporalSocket::ServerHost(int HostPort)
{
	//The port you want the server to listen on
	//int host_port= 1101;
	
	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	//The Windows Socket Version (2.2)
	wVersionRequested = MAKEWORD( 2, 2 );

	//Start up the Windows Socket System
	err = WSAStartup( wVersionRequested, &wsaData );

	//If something went wrong (err != 0) OR the version is not 2.2.  ERROR out
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )) 
	{
		sprintf(LastExeceptionMsg,  "Could not find useable sock dll %d\n",WSAGetLastError());
		return false;
	}

	//Initialize sockets and set any options
	int hsock;
	int * p_int ;

	//Set the hsock Socket
	hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(hsock == -1){
		sprintf(LastExeceptionMsg, "Error initializing socket %d\n",WSAGetLastError());
		return false;
	}
	     
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;

	//Set the sock options
	int SetSockReuse = setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int));

	int SetSockKeepAlive = setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int));

	//If it fails to set the options, exit out
	if( (SetSockReuse == -1)  || (SetSockKeepAlive == -1 ) )
	{
		sprintf(LastExeceptionMsg, "Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return false;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	
	//Set the Socket information (Ineternet protocol 'TCP', The host_port, 0 padding)
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(HostPort);
	 
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	 
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 )
	{
		sprintf(LastExeceptionMsg, "Error binding to socket, make sure nothing else is listening on this port %d\n",WSAGetLastError());
		return false;
	}

	if(listen( hsock, 10) == -1 )
	{
		sprintf(LastExeceptionMsg, "Error listening %d\n",WSAGetLastError());
		return false;
	}

	//Now lets to the server stuff
	
	
	 
	//The Server is now active and waiting for a connection.  
	sprintf(LastExeceptionMsg, "waiting for a connection\n");

	CreateThread(NULL,0, startServerInThread, (VOID*)hsock , 0,0);

	return true;
}

DWORD __stdcall startMethodInThread( LPVOID arg )
{
       if(!arg)
            return 0;
	   TemporalSocket *yc_ptr = (TemporalSocket*)arg;
       yc_ptr->SocketHandler(arg);
       return 1;
}

DWORD __stdcall startServerInThread( LPVOID arg )
{
       if(!arg)
            return 0;
	   TemporalSocket *yc_ptr = (TemporalSocket*)arg;
       yc_ptr->AcceptHandler(arg);
       return 1;
}

//What do do when a connection is recieved and data is present.
DWORD WINAPI TemporalSocket::SocketHandler(VOID* lpParameter)
{
	int *csock = (int*)lpParameter;
	 
	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;

	//Clear the buffer
	memset(buffer, 0, buffer_len);

	//Read the data
	bytecount = recv(*csock, buffer, buffer_len, 0);

	//If Failed to read
	if(bytecount == SOCKET_ERROR)
	{
		sprintf(LastExeceptionMsg, "Error receiving data %d\n", WSAGetLastError());
		
		free(csock);
		return 0;
	}
	
	//Correctly read message
	//sprintf(LastExeceptionMsg, "Received bytes %d\nReceived string \"%s\"\n", bytecount, buffer);

	//React appropriately to the message and send a response back to confirm.
	ParentTemporal->ServerProcessMessage(buffer);
	
	bytecount = send(*csock, buffer, strlen(buffer), 0);

	if(bytecount == SOCKET_ERROR)
	{
		sprintf(LastExeceptionMsg, "Error sending data %d\n", WSAGetLastError());
		
		free(csock);
		return 0;
	}
	   
	//sprintf(LastExeceptionMsg, "Sent bytes %d\n", bytecount);
	
	//cleanup
	free(csock);
	return 0;
}

DWORD WINAPI TemporalSocket::AcceptHandler(VOID* lpParameter)
{
	int hsock = (int)lpParameter;

	int* csock;
	sockaddr_in sadr;
	int    addr_size = sizeof(SOCKADDR);

	while(true)
	{
		csock = (int*)malloc(sizeof(int));
	  
		*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size);

		if(*csock != INVALID_SOCKET )
		{
			//sprintf(LastExeceptionMsg,"Received connection from %s",inet_ntoa(sadr.sin_addr));

			CreateThread(NULL,0, startMethodInThread, (VOID*)csock , 0,0);
		}
		else
		{
			//sprintf(LastExeceptionMsg, "Error accepting %d\n",WSAGetLastError());
		}
	}
}
