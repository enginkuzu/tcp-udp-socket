#include <stdio.h>
#include "libSocket.h"

#define MAX_CLIENT	20

pthread_t threads[MAX_CLIENT];
int sockets[MAX_CLIENT];
char names[MAX_CLIENT][100];

void *run(void *id){
	
	long tid = (long)id;
	int socket = sockets[tid];

	int nameLen = tcpRead(socket, names[tid], 100);
	names[tid][nameLen] = 0;
	printf("USER LOGIN : %s\n", names[tid]);

	char msg[1000];
	while(1){
		int msgLen = tcpRead(socket, msg, 1000);
		if( msgLen <= 0 ){
			printf("USER LOGOFF : %s\n", names[tid]);
			break;
		}
		msg[msgLen] = 0;
		//printf("Thread %ld Read %d byte : ", tid, msgLen);
		printf("%s : %s\n", names[tid], msg);
		
		int i;
		for(i=0;i<MAX_CLIENT;i++){
			if( sockets[i] != -1 && sockets[i] != socket ){
				char *str = ": ";
				int sendLen = strlen(names[tid]) + strlen(msg) + 2;
				char *tmp = (char*)malloc(sendLen);
				memcpy(tmp, names[tid], nameLen);
				memcpy(tmp+nameLen, str, 2);
				memcpy(tmp+nameLen+2, msg, msgLen);
				tcpWrite(sockets[i], tmp, sendLen);
				free(tmp);
			}
		}
	}
	tcpClose(socket);
	sockets[tid] = -1;
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	unsigned int sayac;
	for(sayac=0;sayac<MAX_CLIENT;sayac++)	sockets[sayac] = -1;
	sayac = 0;
	
	int port = 8080;
	int serverSock = tcpServer(port);
	if( serverSock < 0 ){
		puts("SERVER SHUTDOWN");
		exit(1);
	}
	printf("Basic Message Server v0.0.1 STARTED\n");
	printf("All network interfaces ENABLED, Port : %d\n\n", port);
	while(1){
		if( sayac == MAX_CLIENT ){
			printf("Error : MAX_CLIENT limit\n");
			break;
		}
		int clientSock = tcpAccept(serverSock);
		if( clientSock < 0 ){
			usleep(1000);
			continue;
		}
		sockets[sayac] = clientSock;
		//printf("Creating Thread #%d\n", sayac);
		int rc = pthread_create(&threads[sayac], NULL, run, (void *)sayac);
		if(rc){
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			continue;
		}
		sayac++;
	}
	tcpClose(serverSock);
	pthread_exit(NULL);
}
