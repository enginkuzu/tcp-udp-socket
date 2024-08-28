#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


int tcpServer(int port){
	/* Create the TCP socket */
	int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( serverSocket < 0 ){
		perror("SOCKET");
		return -1;
	}
	/* Construct the server sockaddr_in structure */
	struct sockaddr_in serverStruct;
	memset(&serverStruct, 0, sizeof(serverStruct));			/* Clear Struct */
	serverStruct.sin_family			= AF_INET;				/* Internet/IP */
	serverStruct.sin_addr.s_addr	= htonl(INADDR_ANY);	/* Incoming Addr */
	serverStruct.sin_port			= htons(port);			/* Server Port */
	/* Bind the server socket */
	if( bind(serverSocket, (struct sockaddr *) &serverStruct, sizeof(serverStruct)) < 0 ){
		perror("BIND");
		close(serverSocket);
		return -2;
	}
	/* Listen on the server socket : 64 PENDING */
	if( listen(serverSocket, 64) < 0 ){
		perror("LISTEN");
		close(serverSocket);
		return -3;
	}
	//
	return serverSocket;
}

int udpServer(int port, struct sockaddr_in *serverStruct, unsigned int sizeofServerStruct){
	/* Create the TCP socket */
	int serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( serverSocket < 0 ){
		perror("SOCKET");
		return -1;
	}
	/* Construct the server sockaddr_in structure */
	memset(serverStruct, 0, sizeofServerStruct);				/* Clear Struct */
	(*serverStruct).sin_family			= AF_INET;				/* Internet/IP */
	(*serverStruct).sin_addr.s_addr		= htonl(INADDR_ANY);	/* Incoming Addr */
	(*serverStruct).sin_port			= htons(port);			/* Server Port */
	/* Bind the server socket */
	if( bind(serverSocket, (struct sockaddr *)serverStruct, sizeofServerStruct) < 0 ){
		perror("BIND");
		close(serverSocket);
		return -2;
	}
	//
	return serverSocket;
}

int tcpClient(char *serverIp, int port){
	/* Create the TCP socket */
	int clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( clientSocket < 0 ){
		perror("SOCKET");
		return -1;
	}
	/* Construct the server sockaddr_in structure */
	struct sockaddr_in clientStruct;
	memset(&clientStruct, 0, sizeof(clientStruct));			/* Clear Struct */
	clientStruct.sin_family			= AF_INET;				/* Internet/IP */
	clientStruct.sin_addr.s_addr	= inet_addr(serverIp);	/* IP Address */
	clientStruct.sin_port			= htons(port);			/* Server Port */
	/* Establish connection */
	if( connect(clientSocket, (struct sockaddr *) &clientStruct, sizeof(clientStruct)) < 0 ){
		perror("CONNECT");
		close(clientSocket);
		return -2;
	}
	//
	return clientSocket;
}

int udpClient(char *serverIp, int port, struct sockaddr_in *clientStruct, unsigned int sizeofClientStruct){
	/* Create the UDP socket */
	int clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( clientSocket < 0 ){
		perror("SOCKET");
		return -1;
	}
	/* Construct the server sockaddr_in structure */
	memset(clientStruct, 0, sizeofClientStruct);				/* Clear Struct */
	(*clientStruct).sin_family			= AF_INET;				/* Internet/IP */
	(*clientStruct).sin_addr.s_addr		= inet_addr(serverIp);	/* IP Address */	//inet_addr("127.0.0.1");	//htonl(0x7F000001);
	(*clientStruct).sin_port			= htons(port);			/* Server Port */
	//
	return clientSocket;
}

int tcpAccept(int sock){
	int clientSocket;
	struct sockaddr_in clientStruct;
	unsigned int clientlen = sizeof(clientStruct);
	//
	if( (clientSocket = accept(sock, (struct sockaddr *) &clientStruct, &clientlen)) < 0 ){
		perror("ACCEPT");
		return -1;
	}
	printf("Client %s Connected to Socket %d\n", inet_ntoa(clientStruct.sin_addr), clientSocket);
	return clientSocket;
}

int tcpWrite(int sock, void *buffer, unsigned int len){
	int sendlen = send(sock, buffer, len, 0);
	if( sendlen != len ){
		char buf[24];
		sprintf(buf,"SEND TO %d ",sock);
		perror(buf);
		return -1;
	}
#ifdef DEBUG1
    int i;
	printf("< %d byte.\n", sendlen);
	for(i=0;i<sendlen;i++)	printf("%x[%c] ", *(char*)(buffer+i), *(char*)(buffer+i));
	printf("\n");
#endif
	//printf("Socket TCP Write : %d bytes\n", sendlen);
	return sendlen;
}

int udpWrite(int sock, void *buffer, unsigned int len, struct sockaddr_in *clientStruct, unsigned int sizeofClientStruct){
	int sendlen = sendto(sock, buffer, len, 0, (struct sockaddr*)clientStruct, sizeofClientStruct);
	if( sendlen != len ){
		char buf[24];
		sprintf(buf,"SEND TO %d ",sock);
		perror(buf);
		return -1;
	}
	//printf("Socket UDP Write : %d bytes\n", sendlen);
	return sendlen;
}

int tcpRead(int sock, void *buffer, int len){
	len = recv(sock, buffer, len, 0);
	if( len < 0 ){
		char buf[24];
		sprintf(buf,"RECV FROM %d ",sock);
		perror(buf);
		return len;
	}
#ifdef DEBUG1
    int i;
	printf("> %d byte.\n", len);
	for(i=0;i<len;i++)	printf("%x[%c] ", *(char*)(buffer+i), *(char*)(buffer+i));
	printf("\n");
#endif
	//printf("Socket TCP Read : %d bytes\n", len);
	return len;
}

int udpRead(int sock, void *buffer, int len, struct sockaddr_in *clientStruct, unsigned int sizeofClientStruct){
	len = recvfrom(sock, buffer, len, 0, (struct sockaddr*)clientStruct, &sizeofClientStruct);
	if( len < 0 ){
		char buf[24];
		sprintf(buf,"SEND TO %d ",sock);
		perror(buf);
		return -1;
	}
	//printf("Socket UDP Read : %d bytes\n", len);
	return len;
}

int enableBroadcast(int sock){
	int ret;
	int broadcast = 1;
	if( (ret=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast))) < 0 ){
		char buf[34];
		sprintf(buf,"enableBroadcast %d ",sock);
		perror(buf);
	}
	return ret;
}

void tcpClose(int sock){
	close(sock);
}

void udpClose(int sock){
	close(sock);
}

