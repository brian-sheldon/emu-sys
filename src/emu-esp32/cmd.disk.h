
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

cmd_entry_t cmds_disk[] = {
  { "b", disk_b, "", "load boot sector into zero page" },
  { "drv", disk_drv, "[index]", "current drv index or set drv index" },
  { "drvs", disk_drvs, "", "list configured drvs" },
  { "img", disk_img, "[index] [path]", "list img or set img path" },
  { "imgs", disk_imgs, "", "list configured imgs" },
  { "disk", disk_disk, "[trk logsec]", "hexdump of next sector or sector at trk logsec" },
  { "trklog", disk_trklog, "trk logsec", "get blk blksec from trk logsec" },
  { "blksec", disk_blksec, "blk blksec", "get trk logsec from blk blksec" },
  { NULL, NULL, NULL, NULL }
};

