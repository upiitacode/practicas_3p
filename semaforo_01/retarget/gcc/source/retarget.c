#include "retarget_tm4c.h"

#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART_CTL_UARTEN         0x00000001  // UART Enable
#define SYSCTL_RCGC1_UART0      0x00000001  // UART0 Clock Gating Control
#define SYSCTL_RCGC2_GPIOA      0x00000001  // port A Clock Gating Control

void UART0_init(void){
	SYSCTL->RCGC1|= SYSCTL_RCGC1_UART0; 	// Encender periferico UART0
	SYSCTL->RCGC2|= SYSCTL_RCGC2_GPIOA; 	// Encender periferico GPIOA
	UART0->CTL &= ~UART_CTL_UARTEN;      	// Desabilitar UART0 (para configurar)
	UART0->IBRD = 27;                   	// Divisor de frecuencia Entero 
																				// IBRD = int(50,000,000 / (16 * 115200)) = int(27.12673611)
	UART0->FBRD= 8;                    		// Divisor de frecuencia faccional
																				// FBRD = round(0.12673611 * 64) = 8                    
	UART0->LCRH = (UART_LCRH_WLEN_8|UART_LCRH_FEN); // 8 bits de datos (sin sin bit de paridad, un bit de paro)
	UART0->CTL|= UART_CTL_UARTEN;		// habiltar UART
	GPIOA->AFSEL |= 0x03;						// funcion alternativa en PA[1..0]
	GPIOA->DEN |= 0x03;             // usar drivers digitales en PA[1..0]
	GPIOA->PCTL = (GPIOA->PCTL&0xFFFFFF00)+0x00000011;// configurar  PA[1..0] como pines UART
	GPIOA->AMSEL &= ~0x03;          // deshabilitar funciones analogicas en PA[1..0]
}

void uart_send(char c){
	while((UART0->FR&UART_FR_TXFF) != 0){}//esperar hasta que el buffer no este lleno
	UART0->DR = c;//poner dato en el buffer de transmisison
}


void retarget_init()
{
  // Initialize UART
}

int _write (int fd, char *ptr, int len)
{
  /* Write "len" of char from "ptr" to file id "fd"
   * Return number of char written.
   * Need implementing with UART here. */
	int i;
	for(i=0;i<len;i++){
		uart_send(ptr[i]);
	}
  return i;
}

int _read (int fd, char *ptr, int len)
{
  /* Read "len" of char to "ptr" from file id "fd"
   * Return number of char read.
   * Need implementing with UART here. */
  return 0;//len;
}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console
   * Need implementing with UART here. */
	uart_send(ch);
}

/* SystemInit will be called before main */
/*
void SystemInit()
{
    retarget_init();
}
*/
