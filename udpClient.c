#include <stdio.h>
#include <strings.h>
#include "libSocket.h"

int main(){
	struct sockaddr_in clientStruct;
	unsigned int sizeofClientStruct = sizeof(clientStruct);
	
	int clientSock = udpClient("255.255.255.255", 9999, &clientStruct, sizeofClientStruct);
	if( clientSock<0 )	exit(1);
	
	enableBroadcast(clientSock);
	
	// MIN		: 60 - 12(MAC header) - 2(type) - 20(IP header) - 8(UDP header) = 18 byte
	// NORMAL	: 1514 - 12(MAC header) - 2(type) - 20(IP header) - 8(UDP header) = 1472 byte
	// MAX		: 65,535 - 20(IP header) - 8(UDP header) = 65,507 bytes
	char *buffer = "ABC DEF GHI";
	int writen = udpWrite(clientSock, buffer, strlen(buffer), &clientStruct, sizeofClientStruct);
	printf("CLIENT WRITE %d byte\n",writen);
	
	udpClose(clientSock);
	return 0;
}

