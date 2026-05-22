
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

static void disk_b() {
  cpm_disk_rd_log( drvs[0], mem, 0, 0, 1 );
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
    //println( imgs[drvs[i]].getPath() );
  }
}

static void disk_img() {
  int i = drv;
  if ( cmdline.plen > 1 ) {
    i = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    //imgs[i].config( cmdline.p2 );
  }
  print( "img: " );
  print( i );
  print( " path: " );
  //println( imgs[i].getPath() );
}

static void disk_imgs() {
  for ( int i = 0; i < 10; i++ ) {
    print( "drv: " );
    print( drvs[i] );
    print( " img: " );
    print( i );
    print( " path: " );
    //println( imgs[i].getPath() );
  }
}

static void disk_disk() {
  int trk = -1;
  int log = -1;
  if ( cmdline.plen > 1 ) {
    trk = dec2int( cmdline.p1 );
  }
  if ( cmdline.plen > 2 ) {
    log = dec2int( cmdline.p2 );
  }
  uint8_t buffer[128];
  print_cpm_disk_sec_info( drv, trk, log );
  cpm_disk_rd_log( drv, buffer, 0, trk, log );
  print_hex_lines( 0, buffer, 0, 8, 16 );
  strcpy( defcmd, cmdline.p0 );
}

static void disk_trklog() {
  int blk, blksec;
  int trk, log;
  if ( cmdline.plen > 1 ) {
    trk = dec2int( cmdline.p1 );
    if ( cmdline.plen > 2 ) {
      log = dec2int( cmdline.p2 );
      cpm_disk_trklog2blksec( trk, log, &blk, &blksec );
      print( "trk: " );
      print( trk );
      print( " log: " );
      print( log );
      print( " blk: " );
      print( blk );
      print( " blksec: " );
      println( blksec );
    }
  }
}

static void disk_blksec() {
  int blk, blksec;
  int trk, log;
  if ( cmdline.plen > 1 ) {
    blk = dec2int( cmdline.p1 );
    if ( cmdline.plen > 2 ) {
      blksec = dec2int( cmdline.p2 );
      cpm_disk_blksec2trklog( blk, blksec, &trk, &log );
      print( "trk: " );
      print( trk );
      print( " log: " );
      print( log );
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
    disk_load_bin( mem, sizeof( mem ), addr, path );
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

void test_cpm () {
  println( "struct Cpm cpm ..." );
  println( cpm.trks );
  println( cpm.secs );
  println( cpm.secsize );
  println( cpm.trk0 );
  println( cpm.sec0 );
  println( cpm.reserved );
  println( cpm.exts );
  println( cpm.extsize );
  println( cpm.blksecs );
  println( "cpm_log2sec, cpm_sec2log ..." );
  println( cpm_disk_log2sec( 2, 5 ) );
  println( cpm_disk_sec2log( 2, 25 ) );
  println( cpm_disk_log2sec( 1, 5 ) );
  println( cpm_disk_sec2log( 1, 25 ) );
  println( "cpm_disk_isvalid ..." );
  println( cpm_disk_isvalid ( 5, 5 ) );
  println( cpm_disk_isvalid ( 5, 0 ) );
  println( cpm_disk_isvalid ( 77, 5 ) );
  println( cpm_disk_isvalid ( 0, 27 ) );
  println( cpm_disk_isvalid ( 76, 26 ) );
  println( "cpm_disk_pos ..." );
  println( cpm_disk_pos( 0, 1 ) );
  println( cpm_disk_pos( 1, 1 ) );
  println( cpm_disk_pos( 76, 26 ) );
  println( "struct Mon_drv mon_drvs[] ..." );
  println( mon_drvs[0].img );
  println( mon_drvs[0].trk );
  println( mon_drvs[0].log );
  println( cpm_imgs[ mon_drvs[0].img ] );
  println( "cpm_disk_rd_log ..." );
  uint8_t data[128];
  cpm_disk_rd_log( 0, data, 0, 0, 1 );
  print_hex_lines( 0, data, 0, 8, 16 );
  cpm_disk_rd_log( 0, data, 0, -1, -1 );
  print_hex_lines( 0, data, 0, 8, 16 );
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
  { "patch", disk_patch, "", "patches model1.rom to redirect io to serial" },

  { "testcpm", test_cpm, "", "perform tests of disk routines" },

  { NULL, NULL, NULL, NULL }
};

