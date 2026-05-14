
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void cpu_mhz() {
  print( "mhz: " );
  println( mhz );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_on() {
  cpuState.running = true;
  cpuState.on = true;
  println( "cpu: on" );
}

void cpu_off() {
  cpuState.running = false;
  cpuState.on = false;
  println( "cpu: off" );
}

void cpu_stopclr() {
  cpuState.stopped = false;
  cpuState.stopset = false;
}

void cpu_stopset() {
  cpuState.stopset = true;
}

void cpu_stopat() {
  if ( cmdline.plen > 1 ) {
    cpuState.stopat = hex2int( cmdline.p1 ) & 0xffff;
    cpuState.stopset = true;
  }
  print( "stopat set: " );
  print( cpuState.stopset );
  print( " addr: " );
  println( hex4( cpuState.stopat ) );
}

void cpu_pc() {
  if ( cmdline.plen > 1 ) {
    cpu.pc = hex2int( cmdline.p1 ) & 0xffff;
  }
  println( cpu.pc );
}

void regStr( char *buffer, char *reg, int val, int width = 1, int spcs = 1, bool bar = false, bool lf = false ) {
  char numstr[20];
  if ( width == -1 ) {
    snprintf( numstr, sizeof( numstr ), "%d", val );
  } else {
    snprintf( numstr, sizeof( numstr ), "%0*x", width, val );
  }
  strcat( buffer, colors[color].reg_label );
  strcat( buffer, reg );
  strcat( buffer, " " );
  strcat( buffer, colors[color].reg_value );
  strcat( buffer, numstr );
  for ( int i = 0; i < spcs; i++ ) {
    strcat( buffer, " " );
  }
  if ( bar ) {
    strcat( buffer, "|" );
    if ( lf ) {
      strcat( buffer, "\r\n" );
    } else {
      strcat( buffer, " " );
    }
  }
}

int disaddr = 0;

void cpu_l() {
  int addr = disaddr;
  int lines = 8;
  if ( cmdline.plen > 1 ) {
    addr = hex2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    lines = dec2int( cmdline.p2 );
  }
  for ( int i = 0; i < lines; i++ ) {
    addr += dis( mem, addr & 0xffff );
  }
  disaddr = addr;
  strcpy( defcmd, cmdline.p0 );
}

void cpu_state() {
  char buffer[800];
  buffer[0] = '\0';
  //
  regStr( buffer, "PC", cpu.pc, 4, 1, true );
  regStr( buffer, "SP", cpu.sp, 4 );
  regStr( buffer, "A", cpu.a, 2 );
  regStr( buffer, "F", cpu.f, 2, 5, true );
  regStr( buffer, "AF_", cpu.a_<<8 | cpu.f_, 4, 1, true, true );
  //
  regStr( buffer, "BC", cpu.b<<8 | cpu.c, 4, 1, true );
  regStr( buffer, "IR", cpu.i<<8 | cpu.r, 4 );
  regStr( buffer, "s", cpu.f>>7 & 1, 1, 2 );
  regStr( buffer, "z", cpu.f>>6 & 1, 1, 2 );
  regStr( buffer, "h", cpu.f>>4 & 1, 1, 1, true );
  regStr( buffer, "BC_", cpu.b_<<8 | cpu.c_, 4, 1, true, true );
  //
  regStr( buffer, "DE", cpu.d<<8 | cpu.e, 4, 1, true );
  regStr( buffer, "IX", cpu.ix, 4 );
  regStr( buffer, "p", cpu.f>>2 & 1, 1, 2 );
  regStr( buffer, "n", cpu.f>>1 & 1, 1, 2 );
  regStr( buffer, "c", cpu.f & 1, 1, 1, true );
  regStr( buffer, "DE_", cpu.d_<<8 | cpu.d_, 4, 1, true, true );
  //
  regStr( buffer, "HL", cpu.h<<8 | cpu.l, 4, 1, true );
  regStr( buffer, "IY", cpu.iy, 4 );
  regStr( buffer, "iff1", cpu.iff1, 1 );
  regStr( buffer, "iff2", cpu.iff2, 1, 1, true );
  regStr( buffer, "HL_", cpu.h_<<8 | cpu.l_, 4, 1, true, true );
  //
  regStr( buffer, "running", cpuState.running, 1 );
  regStr( buffer, "on", cpuState.on, 1 );
  regStr( buffer, "wait", cpuState.iowait, 1 );
  regStr( buffer, "stop", cpuState.stopped, 1 );
  regStr( buffer, "halt", cpuState.halted, 1 );
  strcat( buffer, "\r\n" );
  //
  regStr( buffer, "ticks", cpuState.ticks, -1 );
  regStr( buffer, "steps", cpuState.steps, -1 );
  //
  println( buffer );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_d() {
  int addr = dumpaddr;
  if ( cmdline.plen > 1 ) {
    addr = hex2int( cmdline.p1 ) & 0xffff;
  }
  println( hexLines( addr, mem, addr, 16, 16 ) );
  dumpaddr = addr + 256;
  strcpy( defcmd, cmdline.p0 );
}

void cpu_next() {
  print( "   " );
  dis( mem, cpu.pc );
}

void cpu_step() {
  bool each = false;
  int loop = 0;
  int loops = 1;
  int ticks = 0;
  if ( cmdline.plen > 1 ) {
    loops = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    each = true;
  }
  if ( cpuState.on == false && cpuState.iowait == false && cpuState.stopped == false && cpuState.halted == false ) {
    cpuState.running = true;
  } else {
    println( "step not taken as cpu is either on or iowait, stopat, halt is not clear ..." );
    cpu_state();
    each = true;
  }
  while ( loop++ < loops ) {
    if ( cpuState.running ) {
      if ( each || loop == loops ) {
        print( "   " );
        dis( mem, cpu.pc );
      }
      ticks += steps();
      if ( each ) {
        cpu_state();
        if ( loop > 0 && loop < loops ) {
          print( colors[color].prompt );
          println( ">>> " );
        }
      }
    }
  }
  if ( ! each ) cpu_state();
  strcpy( defcmd, cmdline.p0 );
}

cmd_entry_t cmds_cpu[] = {
  { "mhz", cpu_mhz, "", "cpu mhz" },
  { "d", cpu_d, "[addr]", "mem dump at next addr or given addr" },
  { "l", cpu_l ,"[addr]" , "disassembly listing for addr" },
  { "step", cpu_step ,"[steps]" , "cpu step once or given steps" },
  { "state", cpu_state ,"" , "cpu state" },
  { "next", cpu_next ,"" , "show next instruction to run" },
  { "stopclr", cpu_stopclr, "", "stopat clr" },
  { "stopset", cpu_stopset, "", "stopat set" },
  { "stopat", cpu_stopat, "[addr]", "stopat addr" },
  { "pc", cpu_pc ,"[addr]" , "show or set reg pc" },
  { "on", cpu_on ,"" , "turn cpu on" },
  { "1b5b317e", cpu_on ,"" , "end key - turn cpu on" },
  { "off", cpu_off ,"" , "turn cpu off" },
  { "1b5b347e", cpu_off ,"" , "home key - turn cpu off" },
  { NULL, NULL, NULL, NULL }
};
