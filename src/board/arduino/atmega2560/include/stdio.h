#ifndef STDIO_H
#define STDIO_H

extern struct Uart * stdio_uart;
void stdio_init(int num, unsigned long baud);

#endif // STDIO_H
