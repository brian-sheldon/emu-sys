#ifdef ESP32
  extern "C" {
    #include "z80.h"
  }
#else
  #include "z80.h"
#endif

// Copyright (C) 2026 Brian Sheldon
//
// MIT License

//
// The #ifdef are mostly being used to stage code as it is being generalized to run on other platforms
//

#include <time.h>

#include "z80.dis.h"

void do_cmd( char *cmd );

static Z80 cpu;

#define MEM_SIZE 0x10000

#ifdef ESP32 
  #define MEM_TRACE_SIZE 0x4000
  #define CPU_TRACE_SIZE 0x4000
#else
  #define MEM_TRACE_SIZE 0x10000
  #define CPU_TRACE_SIZE 0x10000
#endif

#define SYS_INP_QUEUE_SIZE 256

const size_t memSize = MEM_SIZE;
const size_t memMask = MEM_SIZE - 1;
const size_t dataSize = 0x100;
const size_t dataMask = 0xff;

uint8_t mem[ MEM_SIZE ];
uint8_t ports[256];

//
// Trace vars and funcs
//

const size_t traceCpuLen = CPU_TRACE_SIZE;
uint16_t traceCpuStart = 0x0000;
uint8_t traceCpu[ CPU_TRACE_SIZE ];

const size_t traceMemLen = MEM_TRACE_SIZE;
uint16_t traceMemStart = 0x0000;
uint8_t traceMemRd[ MEM_TRACE_SIZE ];
uint8_t traceMemWr[ MEM_TRACE_SIZE ];

void traceCpuClr() {
  for ( size_t i = 0; i < traceCpuLen; i++ ) {
    traceCpu[i] = 0;
  }
}

void traceMemClr() {
  for ( size_t i = 0; i < traceMemLen; i++ ) {
    traceMemRd[i] = 0;
    traceMemWr[i] = 0;
  }
}

//
//
//

const int queueSize = SYS_INP_QUEUE_SIZE;
int queuePos = 0;
char queue[ SYS_INP_QUEUE_SIZE ];

//int drv = 0;
int drvs[] = {0,1,2,3,4,5,6,7,8,9};
#ifdef ESP32
EmuDiskImg imgs[] = {
  EmuDiskImg( "/emu/disks/cpm22-1.dsk" ),
  EmuDiskImg( "/emu/disks/cpm22-2.dsk" ),
  EmuDiskImg( "/emu/disks/8080tools.cpm" ),
  EmuDiskImg( "/emu/disks/trek.cpm" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" ),
  EmuDiskImg( "" )
};
#else
char *imgs[] = {
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  "",
  ""
};
#endif

struct EmuDrive {
  int sides;
  int tracks;
  int sectors;
  int secsize;
  int drv;
  int track;
  int sector;
  int dmalow;
  int dmahigh;
};

struct EmuDrive drive = { 1, 77, 26, 128, 0, 0, 1, 0, 0 };

uint8_t mem_read( void *ctx, uint16_t addr ) {
  (void)ctx;
  if ( cpuState.traceCpu ) {
    if ( addr > traceMemStart && addr < traceMemStart + traceMemLen ) {
      int virtaddr = addr - traceMemStart;
      if ( traceMemRd[virtaddr] < 0xff ) {
        traceMemRd[virtaddr]++;
      }
    }
  }
  return mem[addr];
}

void mem_write( void *ctx, uint16_t addr, uint8_t val ) {
  (void)ctx;
  if ( cpuState.traceCpu ) {
    if ( addr > traceMemStart && addr < traceMemStart + traceMemLen ) {
      int virtaddr = addr - traceMemStart;
      if ( traceMemWr[virtaddr] < 0xff ) {
        traceMemWr[virtaddr]++;
      }
    }
  }
  mem[addr] = val;
}

uint8_t io_read( void *ctx, uint16_t port ) {
  (void)ctx;
  //Serial.print( "io_read port: " );
  //Serial.println( port );
  char ch;
  switch ( port & 0xff ) {
    case 0: // console status input available 0xff input not 0x00
      if ( queuePos > 0 ) {
        return 0xff;
      } else {
        return 0x00;
      }
      break;
    case 1: // console input
      if ( queuePos > 0 ) {
        ch = queue[0];
        for ( int i = 1; i < queuePos; i++ ) {
          queue[i-1] = queue[i];
        }
        queuePos--;
        return (int)ch;
      } else {
        cpuState.iowait = true;
        cpuState.running = false;
        return 0xff;
      }
      break;
    case 2:
      return 0xff;
      break;
    case 4:
      return 0xff;
      break;
    case 5: // aux in
      return 0xff;
      break;
    case 10: // FDC drive
      return drive.drv;
      break;
    case 11: // FDC track
      return drive.track;
      break;
    case 12: // FDC sector
      return drive.sector;
      break;
    case 13: // FDC command IO ready?
      break;
    case 14: // FDC status
      return 0;
      break;
    case 15: // FDC DMA low
      return drive.dmalow;
      break;
    case 16: // FDC DMA high
      return drive.dmahigh;
      break;
    case 0x42: // 0x00 when cmd is finished executing
      return port66Status;
      break;
    default:
      return port & 0xff;
      break;
  }
  return port & 0xff;
}

void io_write( void *ctx, uint16_t port, uint8_t val ) {
  (void)ctx;
  port = port & 0xff;
  char ch[2];
  ch[0] = (char)val;
  ch[1] = '\0';
  int status;
  size_t addr;
  switch ( port & 0xff ) {
    case 1:
      if ( val == 0x0d ) {
        //#ifdef ESP32
          print( ch );
        //#else
          //println( "" );
        //#endif
        //println( "0d" );
      } else if ( val == 0x0a ) {
        //println( "0a" );
        //#ifdef ESP32
          print( ch );
        //#else
        //#endif
      } else {
        print( ch );
      }
      break;
    case 10: // FDC drive
      drive.drv = val;
      break;
    case 11: // FDC track
      drive.track = val;
      break;
    case 12: // FDC sector
      drive.sector = val;
      break;
    case 13: // FDC cmd
      status = 0;
      addr = drive.dmahigh * 256 + drive.dmalow;
      if ( debug_disk ) {
        print_cpm();
        print( "fdc command: " );
        print( val );
        print( " addr: " );
        print( addr );
        print( " drv: " );
        print( drive.drv );
        print( " trk: " );
        print( drive.track );
        print( " sec: " );
        println( drive.sector );
      }
      if ( val == 0 ) {
        cpm_disk_rd_sec( drive.drv, mem, addr, drive.track, drive.sector );
      } else {
        cpm_disk_wr_sec( drive.drv, mem, addr, drive.track, drive.sector );
      }
      break;
    case 15:
      drive.dmalow = val;
      break;
    case 16:
      drive.dmahigh = val;
      break;
    case 66:
      port66Status = 0xff;
      if ( val == 0x42 ) {
        char cmd[100];
        int addr = 0x80;
        int len = mem[addr] - 1;
        if ( len < 1 ) {
          print( "M allows you to run commands" );
          println( " in the monitor without leaving CP/M ..." );
          println( "Usage M [cmd and paramters to exec]" );
        } else {
          addr += 2;
          memcpy( cmd, mem + addr, len );
          cmd[len] = '\0';
          print( "mon exec: " );
          print( "[" );
          print( cmd );
          print( "]" );
          println( "" );
          do_cmd( cmd );
          printclr( colors[color].cpm_color );
        }
        print( colors[color].reset );
        print( "\x1b[1A" );
      }
      port66Status = 0x00;
    break;
    default:
      break;
  }
  if ( status ) {} // use variable to avoid compiler warning
  if ( addr ) {} // use variable to avoid compilter warning
}

uint8_t ops[] = {
  0x01, 0x00, 0x00,       // 00 10 ld bc,0
  0x11, 0x00, 0x00,       // 03 10 ld de,0
  0x21, 0x00, 0x00,       // 06 10 ld hl,0
  // loop avgs ~ 122 ticks, which at 5.20 mhz = a count of about $1000000 every 6 min 33 sec
  // so doing cli cmd d at this time will show 00f0  xxxx 0001 0000, xxxx as this portion changes fast
  // displaying state regularly will show a slightly higher mhz as this value is based only on time
  // running the while loop surrounding the emulation code
  0xed, 0x43, 0xf0, 0x00, // 09 20 ld ($00f0),bc
  0xed, 0x53, 0xf2, 0x00, // 0d 20 ld ($00f2),de
  0xed, 0x63, 0xf4, 0x00, // 11 20 ld ($00f4),hl
  0x03,                   // 15 6 inc bc
  0x78,                   // 16 4 ld a,b
  0xb1,                   // 17 4 or c
  0x20, 0x0a,             // 18 12/7 jr nz,$0024
  0x3e, 0x2e,             // 1a 7 ld a,'.'
  0xd3, 0x01,             // 1c out ($1),a
  0x13,                   // 1e 6 inc de
  0x7a,                   // 1f 4 ld a,d
  0xb3,                   // 20 4 or e
  0x20, 0x01,             // 21 12/7 jr nz,1
  0x23,                   // 23 6 inc hl
  //0xdb, 0x01,             // 11 in a,(0x01)
  //0xd3, 0x02,             // 11 out (0x02),a
  0xc3, 0x09, 0x00,       // 10 jp 9
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00
};

void setupMem() {
  for ( size_t i = 0; i < 65536; i++ ) {
    if ( i < 40 ) {
      mem[i] = ops[i];
    } else {
      mem[i] = 0;
    }
    if ( i < traceCpuLen ) {
      traceCpu[i] = 0;
    }
  }
}

//extern "C" void z80_init( Z80 *cpu );
//extern "C" int Z80_step( Z80 *cpu );

void setupCpu() {
  z80_init( &cpu );
  cpu.mem_read = mem_read;
  cpu.mem_write = mem_write;
  cpu.io_read = io_read;
  cpu.io_write = io_write;
  cpu.ctx = NULL;
  cpu.a = 0;
  cpu.f = 0;
  cpu.sp = 0xfffe;
  cpu.pc = 0x0000;
}

long long ticks = 0;
void setupEmu() {
  setupMem();
  setupCpu();
}

unsigned long lastTime = 0;
long long lastTicks = 0;
float mhz = 0.0;

/*
String status() {
  String s = "";
  s += "status mhz: ";
  s += String( mhz );
  s += " running: ";
  s += String( running );
  return s;
}
*/

int steps( int n ) {
  int ticks = 0;
  int steps = 0;
  // start timer
  while ( cpuState.running && ( ticks < n ) ) {
    uint16_t pc = cpu.pc;
    if ( cpuState.stopset && cpu.pc == cpuState.stopat ) {
      cpuState.stopped = true;
      cpuState.running = false;
    } else {
      int t = z80_step(&cpu);
      if ( t == 1 ) {
        cpuState.halted = true;
        cpuState.running = false;
      } else {
        ticks += t;
        steps++;
        if ( cpuState.traceCpu ) {
          if ( pc > traceCpuStart && pc < traceCpuStart + traceCpuLen ) {
            pc = pc - traceCpuStart;
            if ( traceCpu[pc] < 0xff ) {
              traceCpu[pc]++;
            }
          }
        }
      }
    }
  }
  // stop timer
  // calculate time
  cpuState.ticks += ticks;
  cpuState.steps += steps;
  return ticks;
}

void cpu_frame() {
  int clocks = 50000;
  int ticks = 0;
  if ( cpuState.iowait ) {
    if ( queuePos > 0 ) {
      char ch = queue[0];
      for ( int i = 1; i < queuePos; i++ ) {
        queue[i-1] = queue[i];
      }
      queuePos--;
      cpu.a = (int)ch & 0xff;
      cpuState.iowait = false;
      cpuState.running = true;
    }
  }
  if ( cpuState.running && cpuState.on ) {
    // begin time
    unsigned long delta = 0;
    #ifdef ESP32
      unsigned long beg = 0;
      unsigned long end = 0;
      beg = micros();
    #else
      struct timespec beg, end;
      clock_gettime( CLOCK_MONOTONIC, &beg );
    #endif
    ticks = steps( clocks );
    // end time
    if ( ticks > 30000 ) {
      #ifdef ESP32
        end = micros();
        delta = end - beg;
      #else
        clock_gettime( CLOCK_MONOTONIC, &end );
        long long seconds = end.tv_sec - beg.tv_sec;
        long long nanos = end.tv_nsec - beg.tv_nsec;
        if ( nanos < 0 ) {
          seconds -= 1;
          nanos += 1000000000LL;
        }
        delta = nanos / 1000;
      #endif
      cpuState.mhz = (float)ticks / delta;
      if ( cpuState.mhz < cpuState.mhzMin ) cpuState.mhzMin = cpuState.mhz;
      if ( cpuState.mhz > cpuState.mhzMax ) cpuState.mhzMax = cpuState.mhz;
    }
  }
  cpuState.frames++;
}

/*
void loopEmu() {
  int loopticks = 50000;
  int loops = 10000;
  while ( loops-- > 0 ) {
    if ( cpuState.iowait ) {
      if ( queuePos > 0 ) {
        char ch = queue[0];
        for ( int i = 1; i < queuePos; i++ ) {
          queue[i-1] = queue[i];
        }
        queuePos--;
        cpu.a = (int)ch & 0xff;
        cpuState.iowait = false;
      }
    }
    if ( running && ! cpuState.iowait ) {
      if ( cpuState.stopset && cpuState.stopat == cpu.pc ) {
        cpuState.stopped = true;
      } else {
        cpuState.stopped = false;
        ticks = z80_step(&cpu);
        loopticks = loopticks - ticks;
        cpuState.ticks += ticks;
        cpuState.steps++;
      }
    }
  }
  //
  // calculate the Mhz
  //
  #ifdef ESP32
  unsigned long currTime = micros();
  unsigned long diffTime = currTime - lastTime;
  long diffTicks = ticks - lastTicks;
  if ( diffTime > 0.0 ) {
    mhz = 1.0 * diffTicks / diffTime;
  }
  lastTime = currTime;
  lastTicks = ticks;
  #endif
}
*/


