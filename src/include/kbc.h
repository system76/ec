#ifndef _KBC_H_
#define _KBC_H_

void kbc_init(void);

struct Kbc {
    // Control register
    volatile unsigned char * control;
    // Interrupt control register
    volatile unsigned char * irq;
    // Status register
    volatile unsigned char * status;
    // Keyboard out register
    volatile unsigned char * keyboard_out;
    // Mouse out register
    volatile unsigned char * mouse_out;
    // Data in register
    volatile unsigned char * data_in;
};

extern __code struct Kbc KBC;

#define KBC_STS_OBF (1 << 0)
#define KBC_STS_IBF (1 << 1)
#define KBC_STS_CMD (1 << 3)

unsigned char kbc_status(struct Kbc * kbc);
unsigned char kbc_read(struct Kbc * kbc);
void kbc_keyboard(struct Kbc * kbc, unsigned char data);
void kbc_mouse(struct Kbc * kbc, unsigned char data);

__xdata volatile unsigned char __at(0x1300) KBHICR;
__xdata volatile unsigned char __at(0x1302) KBIRQR;
__xdata volatile unsigned char __at(0x1304) KBHISR;
__xdata volatile unsigned char __at(0x1306) KBHIKDOR;
__xdata volatile unsigned char __at(0x1308) KBHIMDOR;
__xdata volatile unsigned char __at(0x130A) KBHIDIR;

#endif // _KBC_H_
