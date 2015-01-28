/*
	Quadcopter UART Module
*/

#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		
#include <errno.h>
#include <stdlib.h>
#include <string.h>

class UART {

	public:
		int uart0_filestream;
		void create(void);
		~UART();

};

void UART::create(){

	uart0_filestream = -1;

	uart0_filestream = open("/dev/ttyAMA0", O_RDWR);
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}

	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;	
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

}


UART::~UART(){
	if( uart0_filestream != -1 )
		close(uart0_filestream);
}
