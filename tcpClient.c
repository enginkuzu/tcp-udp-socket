#include <stdio.h>
#include "libSocket.h"

int clientSock;
pthread_t readThread[1];

void *run(void *id){
	char msg[1000];
	while(1){
		int readLen = tcpRead(clientSock, &msg, 1000);
		if( readLen <= 0 ){
			printf("\nSERVER GONE !!!\n");
			exit(1);
		}
		msg[readLen] = 0;
		printf("\r%s\nMesaj:", msg);
		fflush(stdout);
	}
	tcpClose(clientSock);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){

	clientSock = tcpClient("127.0.0.1", 8080);
	if( clientSock < 0 ){
		puts("CLIENT SHUTDOWN");
		exit(1);
	}

	int sayi = 0;
	char name[100];
	printf("Lütfen isminizi giriniz:");
	scanf("%s", name);
	tcpWrite(clientSock, name, strlen(name));

	int rc = pthread_create(&readThread, NULL, run, NULL);
	if(rc){
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	char msg[1000];
	while(1){
		printf("Mesaj:");
		scanf("%s", msg);
		tcpWrite(clientSock, msg, strlen(msg));
	}
	tcpClose(clientSock);
	pthread_exit(NULL);
}
