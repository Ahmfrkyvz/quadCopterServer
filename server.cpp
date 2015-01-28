/*
	Quadcopter Server Module
*/

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


class RSocket {
	
	public:
		sockaddr_in server;
		int socket_desc;
		sockaddr_in from;
		socklen_t fromLength;

		void create(int);
		~RSocket();
		//int recvMSG(void);
		//int sendMSG(char*);

};


void RSocket::create(int port)
{	
	printf("  - UDP Socket Creating...\n");

	fromLength = sizeof( from );

	socket_desc = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
  	if ( socket_desc <= 0 )
    	{
      		printf("  - Failed to create socket.\n");
      		exit(EXIT_FAILURE);
    	}
	
	printf("  - Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	bzero(&(server.sin_zero),8);


	//Bind
	if(bind(socket_desc, (const sockaddr *)&server ,sizeof(server)) < 0)
	{
	    printf("  - Binding failed!\n");
	    exit(EXIT_FAILURE);
	}
	printf("  - Bind done.\n\n");
}


RSocket::~RSocket()
{
	if (socket_desc != 0)
	{
		close( socket_desc );
		socket_desc = 0;
	}
}

/*
int RSocket::recvMSG()
{
	int received_bytes = -1;
	if ( socket_desc == 0 )
	{
		printf("Socket is closed.");
		return received_bytes;
	}

	received_bytes = recvfrom(socket_desc, data, PACKETSIZE, 0, (sockaddr*)&from, &fromLength);
	printf("Received bytes = %d\n", received_bytes);

	if (received_bytes == -1 || received_bytes != PACKETSIZE){
		printf("Received bytes = -1\n");
  	}

	return received_bytes;
}

int RSocket::sendMSG(char message[])
{
	//returns -1, if could not send any byte
	return sendto(socket_desc, message, sizeof(message), 0, (sockaddr*) &from, fromLength);
}
*/
