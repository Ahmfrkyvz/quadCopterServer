/*
	----------------------------
	 Quadcopter Project RPi App
	----------------------------
*/

#define VERSION "0.3"

#include <stdio.h>
#include <pthread.h>

#include "server.cpp"
#include "uart.cpp"

#define PACKETSIZE 32
#define PORT 2020


//---------------------------------------------
// global variables
//---------------------------------------------

pthread_t th_udp_recv;	//thread for udp receiver
pthread_t th_udp_send;	//thread for udp sender
pthread_t th_uart;		//thread for uart

RSocket rc;
UART uart;

//data packets
char RecvData[PACKETSIZE];
char SendData[PACKETSIZE];

//receiving packet
int *power;
int *yawParam; //Yaw rounding parameter
int *lrAngle; //left-right angle
int *fbAngle; //front-back angle
int *Kp, *Ki, *Kd; //PID parameters

//sending packet
int *motorFR, *motorFL, *motorBR, *motorBL;
int *ypr[3];


//---------------------------------------------
// Threads
//---------------------------------------------

void* fc_udp_recv (void * arg)
{

	printf("Thread: th_udp_recv created.\n");

	if ( rc.socket_desc == 0 )
	{
		printf("Socket is closed.\n");
		exit(EXIT_FAILURE);
	}

	while(1){

		if( recvfrom(rc.socket_desc, RecvData, PACKETSIZE, 0, (sockaddr*)&(rc.from), &(rc.fromLength)) == PACKETSIZE )
		{
			if(RecvData[0] != 0)
				switch(RecvData[0]){
					case 11: //Send Interupt
						//start
						break;
					case 21: //Close App
						exit(EXIT_SUCCESS);
						break;
					case 23: //Restart RPi
						system("reboot");
						break;
				} //switch
		}

		//show current status:
		printf("-------------------------------------------------------\n");
		printf("Power: %d\t yawParam:%d\n", *power, *yawParam);
		printf("lrAngle: %d\t fbAngle: %d\n", *lrAngle, *fbAngle);
		printf("Kp: %d\t Ki: %d\t Kd: %d\n", *Kp, *Ki, *Kd);
		printf("---------------\n");
		printf("FR: %d\t FL: %d\t BR: %d\t BL: %d\n", *motorFR, *motorFL, *motorBR, *motorBL);
		printf("Yaw: %d\t Pitch: %d\t Roll: %d\n", *ypr[0], *ypr[1], *ypr[2]);

	}//while

	pthread_exit(NULL);
}


void* fc_udp_send (void * arg)
{
	printf("Thread: th_udp_send created.\n");

	while(1){
		usleep(100000);
		sendto(rc.socket_desc, SendData, PACKETSIZE, 0, (sockaddr*) &(rc.from), (rc.fromLength));
	}
	
	pthread_exit(NULL);
}


void* fc_uart (void * arg)
{
	char uartbuffer[PACKETSIZE];
	int offset;
	int rx_length;
	int count;

	printf("Thread: th_uart created.\n");

	while(1)
	{
		usleep(20000);
		if (uart.uart0_filestream != -1)
		{

			//----- Uart Transfer -----
			count = write(uart.uart0_filestream, (void*) RecvData, PACKETSIZE);
			if (count < 0)
			{
				printf("UART Transfer Error\n");
			}

			//----- Uart receiver -----
			offset = 0;
			rx_length = 0;

			do {
				rx_length = read(uart.uart0_filestream, (void*) (uartbuffer + offset), PACKETSIZE);

				offset += rx_length;

				if (rx_length < 0)
				{
					printf ("Receive Error,  error = %s\n", strerror(errno));
					break;
				}
				else if (rx_length == 0)
				{
					printf ("No data\n");
					break;
				}
				else
				{
					if(offset == PACKETSIZE)
						memcpy((void *) SendData, (void *) uartbuffer, PACKETSIZE);
				}
			} while (offset < PACKETSIZE);
			
		}
	
	}

	pthread_exit(NULL);
}


//---------------------------------------------
// main
//---------------------------------------------

int main(int argc, char *argv[])
{
	printf("------------------\n");
	printf("  Quadcopter v%s\n", VERSION);
	printf("------------------\n");

	printf("Port: %d\n", PORT);


	// Initialize server thread
	//--------------------------

	//Create server class
	printf("Server starting...\n");
	rc.create(PORT);

	//pointer definitions
	power = (int*)&RecvData[4];
	yawParam = (int*)&RecvData[8];
	lrAngle = (int*)&RecvData[12];
	fbAngle = (int*)&RecvData[16];
	Kp = (int*)&RecvData[20];
	Ki = (int*)&RecvData[24];
	Kd = (int*)&RecvData[28];

	motorFL = (int*)&SendData[0];
	motorFR = (int*)&SendData[4];
	motorBL = (int*)&SendData[8];
	motorBR = (int*)&SendData[12];
	ypr[0] = (int*)&SendData[20];
	ypr[1] = (int*)&SendData[24];
	ypr[2] = (int*)&SendData[28];

    if(pthread_create(&th_udp_recv, NULL, &fc_udp_recv, NULL) != 0) {
		printf("Creating th_udp_recv thread failed.\n");
		exit(EXIT_FAILURE);
    }

    if(pthread_create(&th_udp_send, NULL, &fc_udp_send, NULL) != 0) {
		printf("Creating th_udp_send thread failed.\n");
		exit(EXIT_FAILURE);
    }


	// Initialize uart thread
	//--------------------------

	//Create uart class
	printf("UART starting...\n");
	uart.create();

	if(pthread_create(&th_uart, NULL, &fc_uart, NULL) != 0) {
		printf("Creating th_uart thread failed.\n");
		exit(EXIT_FAILURE);
    }


    //wait for server
    //pthread_join(th_udp, NULL);

    //wait to quit
    getchar();

	return 0;
}
