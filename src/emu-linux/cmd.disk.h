
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

static void disk_b() {
  cpm_disk_rd_sec( drvs[0], mem, 0, 0, 1 );
  println( "boot sector loaded to zero page ..." );
}

static void disk_drv() {
  if ( cmdline.plen > 1 ) {
    drv = pnum( cmdline.p1, 10 );
  }
  if ( cmdline.plen > 2 ) {
    int i = pnum( cmdline.p2, 10 );
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
    print( mon_drvs[i].img );
    print( " path: " );
    println( cpm_imgs[ mon_drvs[i].img ] );
  }
}

static void disk_img() {
  int i = drv;
  if ( cmdline.plen > 1 ) {
    i = pnum( cmdline.p1, 10 );
  }
  if ( cmdline.plen > 2 ) {
    //_imgs[i].config( cmdline.p2 );
  }
  print( "img: " );
  print( i );
  print( " path: " );
  //println( imgs[i].getPath() );
}

static void disk_imgs() {
  for ( int i = 0; i < 10; i++ ) {
    print( "drv: " );
    print( mon_drvs[i].img );
    print( " img: " );
    print( i );
    print( " path: " );
    println( cpm_imgs[i] );
  }
}

static void disk_disk_pos() {
  if ( cmdline.plen > 2 ) {
    int trk = pnum( cmdline.p1, 10 );
    int sec = pnum( cmdline.p2, 10 );
    int pos = cpm_disk_pos( drv, trk, sec );
    println( pos );
  }
}

static void disk_disk() {
  int trk = -1;
  int log = -1;
  if ( cmdline.plen > 1 ) {
    trk = pnum( cmdline.p1, 10 );
  }
  if ( cmdline.plen > 2 ) {
    log = pnum( cmdline.p2, 10 );
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
    trk = pnum( cmdline.p1, 10 );
    if ( cmdline.plen > 2 ) {
      log = pnum( cmdline.p2, 10 );
      cpm_disk_trklog2blksec( drv, trk, log, &blk, &blksec );
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
    blk = pnum( cmdline.p1, 10 );
    if ( cmdline.plen > 2 ) {
      blksec = pnum( cmdline.p2, 10 );
      cpm_disk_blksec2trklog( drv, blk, blksec, &trk, &log );
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

void disk_cmp() {
  int addr = 0;
  if ( cmdline.plen > 1 ) {
    char *path = cmdline.p1;
    if ( cmdline.plen > 2 ) {
      addr = pnum( cmdline.p2, 16 );
    }
    int filesize = disk_file_size( path );
    uint8_t *buffer = (uint8_t *)malloc( filesize * sizeof(uint8_t) );
    disk_load_bin( buffer, filesize, 0, path );
    for ( int i = 0; i < filesize; i++ ) {
      if ( buffer[i] != mem[ addr + i ] ) {
        print( i );
        print( " : " );
        print( addr + i );
        print( " : " );
        print( buffer[i] );
        print( " : " );
        println( mem[ addr + i ] );
      }
    }
    free( buffer );
  }
}

void disk_load() {
  int addr = 0;
  if ( cmdline.plen > 1 ) {
    char *path = cmdline.p1;
    if ( cmdline.plen > 2 ) {
      addr = pnum( cmdline.p2, 16 );
    }
    int pages = disk_load_bin( mem, sizeof( mem ), addr, path );
    print( "CP/M SAVE command pages: " );
    println( pages );
  }
}

void disk_patch() {
  mem[ 0x33 ] = 0xd3; // out ( 1 ),a
  mem[ 0x34 ] = 0x01; //
  mem[ 0x35 ] = 0xfe; // cp 0d
  //#ifdef ESP32
    mem[ 0x36 ] = 0x0d; //
  //#else
    //mem[ 0x36 ] = 0x0a; //
  //#endif
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

void disk_trs() {
  #ifdef ESP32
    char path[] = "/emu/roms/model1.rom";
  #else
    char path[] = "../../sdcard/emu/roms/model1.rom";
  #endif
  disk_load_bin( mem, sizeof( mem ), 0, path );
  disk_patch();
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
  println( cpm_disk_log2sec( 0, 2, 5 ) );
  println( cpm_disk_sec2log( 0, 2, 25 ) );
  println( cpm_disk_log2sec( 0, 1, 5 ) );
  println( cpm_disk_sec2log( 0, 1, 25 ) );
  println( "cpm_disk_isvalid ..." );
  println( cpm_disk_isvalid ( 0, 5, 5 ) );
  println( cpm_disk_isvalid ( 0, 5, 0 ) );
  println( cpm_disk_isvalid ( 0, 77, 5 ) );
  println( cpm_disk_isvalid ( 0, 0, 27 ) );
  println( cpm_disk_isvalid ( 0, 76, 26 ) );
  println( "cpm_disk_pos ..." );
  println( cpm_disk_pos( 0, 0, 1 ) );
  println( cpm_disk_pos( 0, 1, 1 ) );
  println( cpm_disk_pos( 0, 76, 26 ) );
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
  { "diskpos", disk_disk_pos, "[trk logsec]", "pos within img where sector is located" },
  { "trklog", disk_trklog, "trk logsec", "get blk blksec from trk logsec" },
  { "blksec", disk_blksec, "blk blksec", "get trk logsec from blk blksec" },
  { "load", disk_load, "path [addr]", "load rom/bin into memory at addr" },
  { "cmp", disk_cmp, "path [addr]", "compare bin with memory" },
  { "trs", disk_trs, "path [addr]", "trs load and patch" },
  { "patch", disk_patch, "", "patches model1.rom to redirect io to serial" },

  { "testcpm", test_cpm, "", "perform tests of disk routines" },

  { NULL, NULL, NULL, NULL }
};

