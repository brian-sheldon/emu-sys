
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

//
// Cpu Trace Funcs
//

void trace_cpu_clr() {
  traceCpuClr();
  println( "cpu trace data cleared ..." );
}

void trace_cpu_set() {
  if ( cmdline.plen > 1 ) {
    traceCpuStart = hex2int( cmdline.p1 );
  }
  print( "trace cpu start: " );
  println( hex4( traceCpuStart ) );
}

void trace_cpu_show() {
  int beg = 0x0000;
  int end = 0xffff;
  int over = 0;
  if ( cmdline.plen > 1 ) {
    beg = hex2int( cmdline.p1 ) & 0xffff;
  }
  if ( cmdline.plen > 2 ) {
    end = hex2int( cmdline.p2 ) & 0xffff;
  }
  if ( cmdline.plen > 3 ) {
    over = dec2int( cmdline.p3 ) & 0xffff;
  }
  size_t next = 0;
  for ( int addr = 0; addr < traceCpuLen; addr++ ) {
    if ( addr >= beg && addr <= end ) {
      int count = traceCpu[addr];
      if ( count > over ) {
        int realaddr = traceCpuStart + addr;
        if ( realaddr != next ) {
          print( colors[color].dis_label );
          print( "       pc_" );
          print( hex4( realaddr ) );
          println( ":" );
        }
        print( colors[color].trace_value );
        print( dec0( count, 5 ) );
        print( "  " );
        next = realaddr + dis( mem, realaddr );
      }
    }
  }
}

//
// Mem Trace Funcs
//

void trace_mem_clr() {
  traceMemClr();
  println( "mem trace data cleared ..." );
}

void trace_mem_set() {
  if ( cmdline.plen > 1 ) {
    traceMemStart = hex2int( cmdline.p1 );
  }
  print( "trace mem start: " );
  println( hex4( traceMemStart ) );
}

void trace_mem_show() {
  int beg = 0x0000;
  int end = 0xffff;
  int min = 0;
  if ( cmdline.plen > 1 ) {
    beg = hex2int( cmdline.p1 ) & 0xffff;
  }
  if ( cmdline.plen > 2 ) {
    end = hex2int( cmdline.p2 ) & 0xffff;
  }
  if ( cmdline.plen > 3 ) {
    min = dec2int( cmdline.p3 ) & 0xffff;
  }
  size_t prev = 0;
  for ( int addr = 0; addr < traceMemLen; addr++ ) {
    if ( addr >= beg && addr <= end ) {
      int rd = traceMemRd[addr];
      int wr = traceMemWr[addr];
      int rw = rd + wr;
      int realaddr = traceMemStart + addr;
      if ( rw != 0 ) {
        if ( realaddr != prev + 1 ) {
          print( "pc_" );
          print( hex4( realaddr ) );
          println( ":" );
        }
        prev = realaddr;
      }
      int data = mem[realaddr];
      char ch[2];
      ch[0] = '.';
      ch[1] = '\0';
      if ( data >= 0x20 && data <= 0x7e ) {
        ch[0] = (char)data;
      }
      if ( rw > min ) {
        print( dec0( rw, 3 ) );
        print( "  " );
        print( hex4( realaddr ) );
        print( " " );
        print( hex2( data ) );
        print( " " );
        print( ch );
        print( "  rd: " );
        print( dec0( rd, 3 ) );
        print( "  wr: " );
        println( dec0( wr, 3 ) );
      }
    }
  }
}

//
//
//

cmd_entry_t cmds_trace[] = {
  { "cclr", trace_cpu_clr, "", "clear cpu trace data" },
  { "cset", trace_cpu_set, "[addr]", "set cpu trace addr start" },
  { "cshow", trace_cpu_show, "[beg] [end] [min]", "show num of op fetches at addr over min" },
  { "mclr", trace_mem_clr, "", "clear mem trace data" },
  { "mset", trace_mem_set, "[addr]", "set mem trace addr start" },
  { "mshow", trace_mem_show, "[beg] [end] [min]", "show num of mem rd wr at addr over min" },
  { NULL, NULL, NULL, NULL }
};
