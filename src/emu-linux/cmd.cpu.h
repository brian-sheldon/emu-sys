
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void cpu_mhz() {
  char buffer[20];
  print( "mhz: " );
  snprintf( buffer, sizeof( buffer ), "%.2f", cpuState.mhz );
  print( buffer );
  print( " min: " );
  snprintf( buffer, sizeof( buffer ), "%.2f", cpuState.mhzMin );
  print( buffer );
  print( " max: " );
  snprintf( buffer, sizeof( buffer ), "%.2f", cpuState.mhzMax );
  println( buffer );
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
    cpuState.stopat = pnum( cmdline.p1, 16 ) & 0xffff;
    cpuState.stopset = true;
  }
  print( "stopat set: " );
  print( cpuState.stopset );
  print( " addr: " );
  println( hex4( cpuState.stopat ) );
}

void cpu_pc() {
  if ( cmdline.plen > 1 ) {
    cpu.pc = pnum( cmdline.p1, 16 ) & 0xffff;
  }
  println( cpu.pc );
}

/*
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
*/

int disaddr = 0;

void cpu_l() {
  int addr = disaddr;
  int lines = 8;
  if ( cmdline.plen > 1 ) {
    addr = pnum( cmdline.p1, 16 );
  }
  if ( cmdline.plen > 2 ) {
    lines = pnum( cmdline.p2, 10 );
  }
  for ( int i = 0; i < lines; i++ ) {
    addr += dis( mem, addr & 0xffff );
  }
  disaddr = addr;
  strcpy( defcmd, cmdline.p0 );
}

void printreg( char *reg, int val, int width, int spcs, bool bar, bool lf ) {
  char numstr[20];
  if ( width == -1 ) {
    snprintf( numstr, sizeof( numstr ), "%d", val );
  } else {
    snprintf( numstr, sizeof( numstr ), "%0*x", width, val );
  }
  printclr( colors[color].reg_label );
  print( reg );
  print( " " );
  printclr( colors[color].reg_value );
  print( numstr );
  for ( int i = 0; i < spcs; i++ ) {
    print( " " );
  }
  if ( bar ) {
    print( "|" );
    if ( lf ) {
      println( "" );
    } else {
      print( " " );
    }
  }
}

void cpu_state() {
  //
  printreg( "PC", cpu.pc, 4, 1, true, false );
  printreg( "SP", cpu.sp, 4, 1, false, false );
  printreg( "A", cpu.a, 2, 1, false, false );
  printreg( "F", cpu.f, 2, 5, true, false );
  printreg( "AF_", cpu.a_<<8 | cpu.f_, 4, 1, true, true );
  //
  printreg( "BC", cpu.b<<8 | cpu.c, 4, 1, true, false );
  printreg( "IR", cpu.i<<8 | cpu.r, 4, 1, false, false );
  printreg( "s", cpu.f>>7 & 1, 1, 2, false, false );
  printreg( "z", cpu.f>>6 & 1, 1, 2, false, false );
  printreg( "h", cpu.f>>4 & 1, 1, 1, true, false );
  printreg( "BC_", cpu.b_<<8 | cpu.c_, 4, 1, true, true );
  //
  printreg( "DE", cpu.d<<8 | cpu.e, 4, 1, true, false );
  printreg( "IX", cpu.ix, 4, 1, false, false );
  printreg( "p", cpu.f>>2 & 1, 1, 2, false, false );
  printreg( "n", cpu.f>>1 & 1, 1, 2, false, false );
  printreg( "c", cpu.f & 1, 1, 1, true, false );
  printreg( "DE_", cpu.d_<<8 | cpu.d_, 4, 1, true, true );
  //
  printreg( "HL", cpu.h<<8 | cpu.l, 4, 1, true, false );
  printreg( "IY", cpu.iy, 4, 1, false, false );
  printreg( "iff1", cpu.iff1, 1, 1, false, false );
  printreg( "iff2", cpu.iff2, 1, 1, true, false );
  printreg( "HL_", cpu.h_<<8 | cpu.l_, 4, 1, true, true );
  //
  printreg( "running", cpuState.running, 1, 1, false, false );
  printreg( "on", cpuState.on, 1, 1, false, false );
  printreg( "wait", cpuState.iowait, 1, 1, false, false );
  printreg( "stop", cpuState.stopped, 1, 1, false, false );
  printreg( "halt", cpuState.halted, 1, 1, false, false );
  println( "" );
  //
  printreg( "ticks", cpuState.ticks, -1, 1, false, false );
  printreg( "steps", cpuState.steps, -1, 1, false, false );
  //
  println( "" );
  strcpy( defcmd, cmdline.p0 );
}

/*
void cpu_state_old() {
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
*/

void cpu_d() {
  int addr = dumpaddr;
  if ( cmdline.plen > 1 ) {
    addr = pnum( cmdline.p1, 16 ) & 0xffff;
  }
  println( hexLines( addr, mem, addr, 16, 16 ) );
  dumpaddr = addr + 256;
  strcpy( defcmd, cmdline.p0 );
}

void cpu_next() {
  print( "   " );
  dis( mem, cpu.pc );
}

void cpu_steps( int loops, bool each ) {
  int loop = 0;
  int ticks = 0;
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
      ticks += steps( 1 );
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
}

void cpu_step() {
  bool each = false;
  int loop = 0;
  int loops = 1;
  int ticks = 0;
  if ( cmdline.plen > 1 ) {
    loops = pnum( cmdline.p1, 10 );
  }
  if ( cmdline.plen > 2 ) {
    each = true;
  }
  cpu_steps( loops, each );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_step100() {
  cpu_steps( 100, false );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_step1k() {
  cpu_steps( 1000, false );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_step10k() {
  cpu_steps( 10000, false );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_step100k() {
  cpu_steps( 100000, false );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_step1m() {
  cpu_steps( 1000000, false );
  strcpy( defcmd, cmdline.p0 );
}

void cpu_sendkey( char ch ) {
  if ( queuePos < queueSize ) {
    #ifndef ESP32
      if ( ch == 0x0a ) {
        ch = 0x0d;
      }
    #endif
    queue[queuePos++] = ch;
  }
}

void cpu_sendchrs( char *chrs, bool space ) {
  if ( space ) cpu_sendkey( ' ' );
  int len = strlen( chrs );
  for ( int i = 0; i < len; i++ ) {
    char ch = chrs[i];
    cpu_sendkey( ch );
  }
}

void cpu_sendcr() {
  cpu_sendkey( 0x0d );
}

void cpu_sendstr() {
  if ( cmdline.plen > 1 ) cpu_sendchrs( cmdline.p1, false );
  if ( cmdline.plen > 2 ) cpu_sendchrs( cmdline.p2, true );
  if ( cmdline.plen > 3 ) cpu_sendchrs( cmdline.p3, true );
  if ( cmdline.plen > 4 ) cpu_sendchrs( cmdline.p4, true );
  if ( cmdline.plen > 5 ) cpu_sendchrs( cmdline.p5, true );
}

void cpu_sendln() {
  cpu_sendstr();
  cpu_sendcr();
}

cmd_entry_t cmds_cpu[] = {
  { "mhz", cpu_mhz, "", "cpu mhz" },
  { "d", cpu_d, "[addr]", "mem dump at next addr or given addr" },
  { "l", cpu_l ,"[addr]" , "disassembly listing for addr" },
  { "step", cpu_step ,"[steps]" , "cpu step once or given steps" },
  { "step100", cpu_step100 ,"" , "cpu step 100" },
  { "step1k", cpu_step1k ,"" , "cpu step 1,000" },
  { "step10k", cpu_step10k ,"" , "cpu step 10,000" },
  { "step100k", cpu_step100k ,"" , "cpu step 100,000" },
  { "step1m", cpu_step1m ,"" , "cpu step 1,000,000" },
  { "sendstr", cpu_sendstr, "str", "send str to system" },
  { "sendln", cpu_sendln, "[str]", "send str plus cr to system" },
  { "state", cpu_state ,"" , "cpu state" },
  { "next", cpu_next ,"" , "show next instruction to run" },
  { "stopclr", cpu_stopclr, "", "stopat clr" },
  { "stopset", cpu_stopset, "", "stopat set" },
  { "stopat", cpu_stopat, "[addr]", "stopat addr" },
  { "pc", cpu_pc ,"[addr]" , "show or set reg pc" },
  { "on", cpu_on ,"" , "turn cpu on (Home key)" },
  { "off", cpu_off ,"" , "turn cpu off (End key)" },
  { NULL, NULL, NULL, NULL }
};
