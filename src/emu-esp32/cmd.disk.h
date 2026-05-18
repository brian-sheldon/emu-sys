
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

static void disk_b() {
  imgs[drvs[0]].readsec( mem, 0, 0, 1 );
  println( "boot sector loaded to zero page ..." );
}

static void disk_drv() {
  if ( cmdline.plen > 1 ) {
    drv = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    int i = dec2int( cmdline.p2 );
    drvs[drv] = i;
  }
  print( "drv: " );
  println( drv );
}

static void disk_drvs() {
  for ( int i = 0; i < 10; i++ ) {
    print( "drv: " );
    print( i );
    print( " img: " );
    print( drvs[i] );
    print( " path: " );
    println( imgs[drvs[i]].getPath() );
  }
}

static void disk_img() {
  int i = drv;
  if ( cmdline.plen > 1 ) {
    i = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    imgs[i].config( cmdline.p2 );
  }
  print( "img: " );
  print( i );
  print( " path: " );
  println( imgs[i].getPath() );
}

static void disk_imgs() {
  for ( int i = 0; i < 10; i++ ) {
    print( "drv: " );
    print( drvs[i] );
    print( " img: " );
    print( i );
    print( " path: " );
    println( imgs[i].getPath() );
  }
}

static void disk_disk() {
  int track = -1;
  int logical = -1;
  if ( cmdline.plen > 1 ) {
    track = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    logical = dec2int( cmdline.p2 );
  }
  uint8_t buffer[128];
  imgs[drv].readlog( buffer, 0, track, logical );
  println( imgs[drv].secinfo( drv, track, logical ) );
  println( hexLines( 0, buffer, 0, 8, 16 ) );
  imgs[drv].next();
  strcpy( defcmd, cmdline.p0 );
}

static void disk_trklog() {
  int blk, blksec;
  int trk, log, sec;
  if ( cmdline.plen > 1 ) {
    trk = dec2int( cmdline.p1 );
    if ( cmdline.plen > 2 ) {
      log = dec2int( cmdline.p2 );
      auto [ sec, blk, blksec ] = imgs[drvs[drv]].trksec( trk, log );
      print( "trk: " );
      print( trk );
      print( " log: " );
      print( log );
      print( " sec: " );
      print( sec );
      print( " blk: " );
      print( blk );
      print( " blksec: " );
      println( blksec );
    }
  }
}

static void disk_blksec() {
  int blk, blksec;
  int trk, log, sec;
  if ( cmdline.plen > 1 ) {
    blk = dec2int( cmdline.p1 );
    if ( cmdline.plen > 2 ) {
      blksec = dec2int( cmdline.p2 );
      auto [ trk, log, sec ] = imgs[drvs[drv]].blksec( blk, blksec );
      print( "trk: " );
      print( trk );
      print( " log: " );
      print( log );
      print( " sec: " );
      print( sec );
      print( " blk: " );
      print( blk );
      print( " blksec: " );
      println( blksec );
    }
  }
}

void disk_load() {
  int addr = 0;
  if ( cmdline.plen > 1 ) {
    char *path = cmdline.p1;
    if ( cmdline.plen > 2 ) {
      addr = hex2int( cmdline.p2 );
    }
    EmuFile file = EmuFile( path, true );
    file.open();
    size_t size = file.size();
    if ( ( addr + size < 0xffff ) ) {
      file.seek( 0 );
      file.read( mem + addr, size );
      print( "Binary size: " );
      print( size );
      print( " loaded at addr: " );
      println( addr );
    } else {
      print( "Binary size: " );
      print( size );
      print( " too large to load at addr: " );
      println( addr );
    }
    file.close();
    diskActivity( true );
  }
}

void disk_patch() {
  mem[ 0x33 ] = 0xd3; // out ( 1 ),a
  mem[ 0x34 ] = 0x01; //
  mem[ 0x35 ] = 0xfe; // cp 0d
  mem[ 0x36 ] = 0x0d; //
  mem[ 0x37 ] = 0xc0; // ret nz
  mem[ 0x38 ] = 0x3e; // ld a,0a
  mem[ 0x39 ] = 0x0a; //
  mem[ 0x3a ] = 0xd3; // out (1),a
  mem[ 0x3b ] = 0x01; //
  mem[ 0x3c ] = 0xc9; // ret
  mem[ 0x49 ] = 0xdb; // in a,(1)
  mem[ 0x4a ] = 0x01; //
  mem[ 0x4b ] = 0xc9; // ret
}

cmd_entry_t cmds_disk[] = {
  { "b", disk_b, "", "load boot sector into zero page" },
  { "drv", disk_drv, "[index]", "current drv index or set drv index" },
  { "drvs", disk_drvs, "", "list configured drvs" },
  { "img", disk_img, "[index] [path]", "list img or set img path" },
  { "imgs", disk_imgs, "", "list configured imgs" },
  { "disk", disk_disk, "[trk logsec]", "hexdump of next sector or sector at trk logsec" },
  { "trklog", disk_trklog, "trk logsec", "get blk blksec from trk logsec" },
  { "blksec", disk_blksec, "blk blksec", "get trk logsec from blk blksec" },
  { "load", disk_load, "path [addr]", "load rom/bin into memory at addr" },
  { "patch", disk_patch, "", "patches loaded model1.rom to redirect input/output" },
  { NULL, NULL, NULL, NULL }
};

