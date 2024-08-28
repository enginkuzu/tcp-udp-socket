#include <stdio.h>
#include "libSocket.h"

int main(){
	struct sockaddr_in serverStruct;
	unsigned int sizeofServerStruct = sizeof(serverStruct);
	
	int serverSock = udpServer(9999, &serverStruct, sizeofServerStruct);
	if( serverSock<0 )	exit(1);
	
	// MIN		: 60 - 12(MAC header) - 2(type) - 20(IP header) - 8(UDP header) = 18 byte
	// NORMAL	: 1514 - 12(MAC header) - 2(type) - 20(IP header) - 8(UDP header) = 1472 byte
	// MAX		: 65,535 - 20(IP header) - 8(UDP header) = 65,507 bytes
	char buffer[70000];
	while(1){
		// NOT : udpRead gecikse bile veri arkada okunmayı bekler!
		// NOT : Veri büyüklüğünden daha az byte okunursa verinin okunmayan kısmı çöpe gider!
		int readed = udpRead(serverSock, buffer, 1472, &serverStruct, sizeofServerStruct);
		if( readed<0 )	break;
		buffer[readed] = 0;
		printf("SERVER READ %d byte [%s]\n",readed,buffer);
	}
	udpClose(serverSock);
	return 0;
}

