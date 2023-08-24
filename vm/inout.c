#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#if 0
#include <asm/sigcontext.h>
#endif

#include <ucontext.h>

unsigned char inb(int port) {
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inb %dx, %al");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return v & 0xff;
}

unsigned short inw(int port) {
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inw %dx, %al");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return (v >> 16) & 0xffff;
}

unsigned int inl(int port) {
  int v;
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("inl %0, %eax");
  asm volatile ("mov %%eax, %0" : "=r"(v) :);
  return v;
}

void outb(int port, int v) {
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outb %al, %dx");
}

void outw(int port, int v) {
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outb %ax, %dx");
}

void outl(int port, int v) {
  port <<= 16;
  asm volatile ("mov %0, %%edx" :: "r"(port));
  asm volatile ("mov %0, %%eax" :: "r"(v));
  asm volatile ("outb %eax, %dx");
}

void inout() {
  int i;
  for (int i = 0; i < 256; i++) {
    printf("PORT[%d]:", i);
    outb(i, i); printf(" (byte)%02x", inb(i));
    outw(i, i); printf(" (word)%04x", inw(i));
    outb(i, i); printf(" (long)%08x", inl(i));
    printf("\n");
  }
}

struct sigframe {
#if defined(__i386__)
  int pretcode;
  int sig;
#endif
#if defined(__amd64__)
  long dummy[6];
#endif
  struct ucontext_t sc;
  // struct sigcontext *sc;
};


void sigsegv(int val) {
  long *sp;
  unsigned char *pc;
  volatile struct sigframe *frame;
  static unsigned int port[0x10000];

  sp = __builtin_frame_address(0);
  frame = (struct sigframe *)(sp + 1);

#if defined(__i386__)
#define xDX edx
#define xAX eax
#define xIP eip
#endif
#if defined(__amd64__)
#define xDX rdx
#define xAX rax
#define xIP rip
#endif

#if 0
  printf("SP\t: %16lx\n", (long)sp);
  printf("FRAME\t: %16lx\n", (long)frame);
  printf("DX\t: %16lx\n", (long)frame->sc.xDX);
  printf("AX\t: %16lx\n", (long)frame->sc.xAX);
  printf("IP\t: %16lx\n", (long)frame->sc.xIP);
  exit(0);
#endif

  pc = (unsigned char *)frame->sc.xIP;


}
