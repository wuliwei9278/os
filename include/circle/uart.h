#ifndef _rpvm_uart_h
#define _rpvm_uart_h

void uart_init();

void uart_putc(unsigned char c);
 
unsigned char uart_getc();
 
void uart_puts(const char* str);
 
#endif