extern "C" {
  #include "z80.h"
}

// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "z80.dis.h"

void do_cmd( char *cmd );

static Z80 cpu;

const size_t memSize = 0x10000;
const size_t memMask = 0xffff;
const size_t dataSize = 0x100;
const size_t dataMask = 0xff;

byte mem[ memSize ];
byte ports[256];

//
// Trace vars and funcs
//

const size_t traceCpuLen = 0x4000;
size_t traceCpuStart = 0x0000;
uint16_t traceCpu[ traceCpuLen ];

const size_t traceMemLen = 0x4000;
size_t traceMemStart = 0x0000;
uint8_t traceMemRd[ traceMemLen ];
uint8_t traceMemWr[ traceMemLen ];

void traceCpuClr() {
  for ( int i = 0; i < traceCpuLen; i++ ) {
    traceCpu[i] = 0;
  }
}

void traceMemClr() {
  for ( int i = 0; i < traceMemLen; i++ ) {
    traceMemRd[i] = 0;
    traceMemWr[i] = 0;
  }
}

//
//
//

const int queueSize = 256;
int queuePos = 0;
char queue[queueSize];

//int drv = 0;
int drvs[] = {0,1,2,3,4,5,6,7,8,9};
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

struct EmuDrive {
  int sides = 1;
  int tracks = 77;
  int sectors = 26;
  int secsize = 128;
  int drv = 0;
  int track = 0;
  int sector = 1;
  int dmalow = 0;
  int dmahigh = 0;
};

EmuDrive drive;

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
  port = port & 0xff;
  //Serial.print( "io_write port: " );
  //Serial.print( port );
  //Serial.print( " val: " );
  //Serial.println( val );
  int status, addr;
  switch ( port & 0xff ) {
    case 1:
      Serial.print( (char)val );
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
      if ( val == 0 ) {
        imgs[drvs[drive.drv]].readsec( mem, addr, drive.track, drive.sector );
      } else {
        //imgs[drvs[drive.drv]].writesec( mem, addr, drive.track, drive.sector );
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
          println();
          do_cmd( cmd );
        }
        print( colors[color].reset );
        print( "\x1b[1A" );
      }
      port66Status = 0x00;
    break;
    default:
      break;
  }
}

byte ops[] = {
  0x01, 0x00, 0x00,       // 10 ld bc,0
  0x11, 0x00, 0x00,       // 10 ld de,0
  0x21, 0x00, 0x00,       // 10 ld hl,0
  // loop avgs ~ 122 ticks, which at 5.20 mhz = a count of about $1000000 every 6 min 33 sec
  // so doing cli cmd d at this time will show 00f0  xxxx 0001 0000, xxxx as this portion changes fast
  // displaying state regularly will show a slightly higher mhz as this value is based only on time
  // running the while loop surrounding the emulation code
  0xed, 0x43, 0xf0, 0x00, // 20 ld ($00f0),bc
  0xed, 0x53, 0xf2, 0x00, // 20 ld ($00f2),de
  0xed, 0x63, 0xf4, 0x00, // 20 ld ($00f4),hl
  0x03,                   //  6 inc bc
  0x78,                   //  4 ld a,b
  0xb1,                   //  4 or c
  0x20, 0x03,             // 12/7 jr nz,6
  0x13,                   //  6 inc de
  0x7a,                   //  4 ld a,d
  0xb3,                   //  4 or e
  0x20, 0x01,             // 12/7 jr nz,1
  0x23,                   //  6 inc hl
  0xdb, 0x01,             // 11 in a,(0x01)
  0xd3, 0x02,             // 11 out (0x02),a
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
  int i;
  for ( int i = 0; i < 65536; i++ ) {
    if ( i < 40 ) {
      mem[i] = ops[i];
    } else {
      mem[i] = 0;
    }
    if ( i >= 0 && i < traceCpuLen ) {
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

String status() {
  String s = "";
  s += "status mhz: ";
  s += String( mhz );
  s += " running: ";
  s += String( running );
  return s;
}

int steps( int n = 1 ) {
  int ticks = 0;
  int steps = 0;
  // start timer
  while ( cpuState.running && ( ticks < n ) ) {
    int pc = cpu.pc;
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
            if ( traceCpu[pc] < 0xffff ) {
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
  if ( cpuState.iowait ) {
    if ( queuePos > 0 ) {
      char ch = queue[0];
      for ( int i = 0; i < queuePos; i++ ) {
        queue[i-1] = queue[i];
      }
      queuePos--;
      cpu.a = (int)ch & 0xff;
      cpuState.iowait = false;
      cpuState.running = true;
    }
  }
  if ( cpuState.running && cpuState.on ) {
    int ticks = steps( clocks );
  }
  //frames++;
}

void loopEmu() {
  int loopticks = 50000;
  int loops = 10000;
  while ( loops-- > 0 ) {
    if ( cpuState.iowait ) {
      if ( queuePos > 0 ) {
        char ch = queue[0];
        for ( int i = 0; i < queuePos; i++ ) {
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
  unsigned long currTime = micros();
  unsigned long diffTime = currTime - lastTime;
  long diffTicks = ticks - lastTicks;
  if ( diffTime > 0.0 ) {
    mhz = 1.0 * diffTicks / diffTime;
  }
  lastTime = currTime;
  lastTicks = ticks;
}



