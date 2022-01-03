//
// low-level driver routines for pl011 UART.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "aarch64.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((volatile uint32 *)(UART0 + reg))

// the UART control registers.
// pl011
#define DR  0x00
#define FR  0x18
#define FR_RXFE (1<<4)  // recieve fifo empty
#define FR_TXFF (1<<5)  // transmit fifo full
#define FR_RXFF (1<<6)  // recieve fifo full
#define FR_TXFE (1<<7)  // transmit fifo empty
#define IBRD  0x24
#define FBRD  0x28
#define LCRH  0x2c
#define LCRH_FEN  (1<<4)
#define LCRH_WLEN_8BIT  (3<<5)
#define CR    0x30
#define IMSC  0x38
#define INT_RX_ENABLE (1<<4)
#define INT_TX_ENABLE (1<<5)
#define ICR   0x44

#define ReadReg(reg) (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the transmit output buffer.
struct spinlock uart_tx_lock;
#define UART_TX_BUF_SIZE 32
char uart_tx_buf[UART_TX_BUF_SIZE];
uint64 uart_tx_w; // write next to uart_tx_buf[uart_tx_w % UART_TX_BUF_SIZE]
uint64 uart_tx_r; // read next from uart_tx_buf[uart_tx_r % UART_TX_BUF_SIZE]

extern volatile int panicked; // from printf.c

void uartstart();

void
uartinit(void)
{
  // disable uart
  WriteReg(CR, 0);

  // disable interrupts.
  WriteReg(IMSC, 0);

  // in qemu, it is not necessary to set baudrate.

  // enable FIFOs.
  // set word length to 8 bits, no parity.
  WriteReg(LCRH, LCRH_FEN | LCRH_WLEN_8BIT);

  // enable RXE, TXE and enable uart.
  WriteReg(CR, 0x301);

  // enable transmit and receive interrupts.
  WriteReg(IMSC, INT_RX_ENABLE | INT_TX_ENABLE);

  initlock(&uart_tx_lock, "uart");
}

// add a character to the output buffer and tell the
// UART to start sending if it isn't already.
// blocks if the output buffer is full.
// because it may block, it can't be called
// from interrupts; it's only suitable for use
// by write().
void
uartputc(int c)
{
  acquire(&uart_tx_lock);

  if(panicked){
    for(;;)
      ;
  }

  while(1){
    if(uart_tx_w == uart_tx_r + UART_TX_BUF_SIZE){
      // buffer is full.
      // wait for uartstart() to open up space in the buffer.
      sleep(&uart_tx_r, &uart_tx_lock);
    } else {
      uart_tx_buf[uart_tx_w % UART_TX_BUF_SIZE] = c;
      uart_tx_w += 1;
      uartstart();
      release(&uart_tx_lock);
      return;
    }
  }
}

// alternate version of uartputc() that doesn't 
// use interrupts, for use by kernel printf() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
void
uartputc_sync(int c)
{
  push_off();

  if(panicked){
    for(;;)
      ;
  }

  // wait for ... TODO: comment */
  while(ReadReg(FR) & FR_TXFF)
    ;
  WriteReg(DR, c);

  pop_off();
}

// if the UART is idle, and a character is waiting
// in the transmit buffer, send it.
// caller must hold uart_tx_lock.
// called from both the top- and bottom-half.
void
uartstart()
{
  while(1){
    if(uart_tx_w == uart_tx_r){
      // transmit buffer is empty.
      return;
    }
    
    if(ReadReg(FR) & FR_TXFF){
      // the UART transmit holding register is full,
      // so we cannot give it another byte.
      // it will interrupt when it's ready for a new byte.
      return;
    }
    
    int c = uart_tx_buf[uart_tx_r % UART_TX_BUF_SIZE];
    uart_tx_r += 1;
    
    // maybe uartputc() is waiting for space in the buffer.
    wakeup(&uart_tx_r);
    
    WriteReg(DR, c);
  }
}

// read one input character from the UART.
// return -1 if none is waiting.
int
uartgetc(void)
{
  if(ReadReg(FR) & FR_RXFE)
    return -1;
  else
    return ReadReg(DR);
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from trap.c.
void
uartintr(void)
{
  // read and process incoming characters.
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }

  // send buffered characters.
  acquire(&uart_tx_lock);
  uartstart();
  release(&uart_tx_lock);

  // clear transmit and receive interrupts.
  WriteReg(ICR, INT_RX_ENABLE|INT_TX_ENABLE);
}
