
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#ifdef ESP32

#include <SPI.h>
#include <SD.h>
#include <FS.h>

#include <tuple>

#else
  #include <stdio.h>
#endif

#ifdef ESP32

class EmuFileUtil {
  private:
    String filename;
    bool listopen = false;
    File listfile;
  public:
    EmuFileUtil() {}
    String* list( String path ) {
      int fc = 0;
      static String files[100];
      File dir = SD.open( path );
      if ( dir ) {
        File next = dir.openNextFile();
        while ( next && fc < 99 ) {
          files[fc++] = next.name();
          next = dir.openNextFile();
        }
      }
      files[fc] = "";
      return files;
    }
    void ls( String path ) {
      File dir = SD.open( path );
      File next = dir.openNextFile();
      while ( next ) {
        bool isDir = next.isDirectory();
        String name = next.name();
        unsigned long size = next.size();
        if ( isDir ) {
          print( colors[color].ls_dir );
        } else {
          print( colors[color].ls_file );
        }
        if ( isDir ) print( "[" );
        print( name );
        if ( ! isDir ) {
          print( "  " );
          print( size );
        }
        if ( isDir ) print( "]" );
        println();
        next.close();
        next = dir.openNextFile();
      }
      diskActivity();
      dir.close();
    }
    void mkdir( String path ) {
      SD.mkdir( path );
      diskActivity();
      diskActivity( true );
    }
    void rmdir( String path ) {
      SD.rmdir( path );
      diskActivity();
      diskActivity( true );
    }
    void mv( String srcpath, String dstpath ) {
      //
    }
    void rm( String path ) {
      SD.remove( path );
      diskActivity();
      diskActivity( true );
    }
    void cp( String srcpath, String dstpath ) {
      File src = SD.open( srcpath, FILE_READ );
      File dst = SD.open( dstpath, FILE_WRITE );
      if ( src && dst ) {
        uint8_t buffer[512];
        int count = 0;
        while ( src.available() > 0 ) {
          size_t bytes = src.read( buffer, sizeof( buffer ) );
          dst.write( buffer, bytes );
          print( "." );
          count++;
          if ( ( count % 64 ) == 0 ) {
            println();
          }
        }
        src.close();
        dst.close();
        println();
        println( "Copy complete ..." );
        diskActivity();
        diskActivity( true );
      } else {
        println( "Error opening files ..." );
      }
    }
    void cmp( String srcpath, String dstpath ) {
      File src0 = SD.open( srcpath, FILE_READ );
      File src1 = SD.open( dstpath, FILE_READ );
      if ( src0 && src1 ) {
        uint8_t buffer0[512];
        uint8_t buffer1[512];
        int count = 0;
        bool res = true;
        while ( src0.available() > 0 ) {
          size_t bytes0 = 0;
          size_t bytes1 = 0;
          bytes0 = src0.read( buffer0, sizeof( buffer0 ) );
          if ( src1.available() > 0 ) {
            bytes1 = src1.read( buffer1, sizeof( buffer1 ) );
          }
          if ( bytes0 == bytes1 ) {
            for ( int i = 0; i < bytes0; i++ ) {
              if ( buffer0[i] != buffer1[i] ) {
                res = false;
              }
            }
          } else {
            res = false;
          }
          print( "." );
          count++;
          if ( ( count % 64 ) == 0 ) {
            println();
          }
        }
        src0.close();
        src1.close();
        println();
        diskActivity();
        print( "Compare complete ... res: " );
        println( res );
      } else {
        println( "Error opening files ..." );
      }
    }
};

class EmuFile {
  private:
    String path;
    bool readonly;
    File file;
  public:
    EmuFile( String path = "", bool readonly = false ) {
      this->path = path;
      this->readonly = readonly;
    }
    void setPath( String path ) {
      this->path = path;
    }
    void setReadonly( bool readonly ) {
      this->readonly = readonly;
    }
    bool isOpen() {
      if ( this->file ) {
        return true;
      } else {
        return false;
      }
    }
    String getPath() {
      return this->path;
    }
    bool isDirectory() {
      if ( this->file ) {
        return this->file.isDirectory();
      }
    }
    String nextFile() {
      String name = "";
      if ( this->file ) {
        File next = this->file.openNextFile();
        if ( next ) {
          name = next.name();
          next.close();
        }
      }
      return name;
    }
    bool open() {
      if ( this->readonly ) {
        this->file = SD.open( this->path, FILE_READ );
      } else {
        this->file = SD.open( this->path, "r+w" );
      }
      return this->isOpen();
    }
    unsigned long size() {
      if ( this->file ) {
        return this->file.size();
      }
    }
    unsigned long pos() {
      if ( this->file ) {
        return this->file.position();
      }
    }
    void seek( unsigned long pos ) {
      if ( this->file ) {
        this->file.seek( pos );
      }
    }
    size_t read( uint8_t *data, size_t size ) {
      if ( this->file ) {
        size_t bytes = this->file.read( data, size );
        diskActivity();
        return bytes;
      } else {
        return 0;
      }
    }
    void write( uint8_t *data, size_t size ) {
      if ( this->file ) {
        this->file.write( data, size );
        this->file.flush();
        diskActivity( true );
      }
    }
    void close() {
      if ( this->file ) {
        this->file.close();
      }
    }
};

class EmuDiskImg {
  private:
    String path;
    int sides, tracks, sectors, secsize;
    bool readonly;
    EmuFile file;
    bool isOpen = false;
    int track, sector, logical;
    int blksize, blktrk;
  public:
    EmuDiskImg( String path = "", int sides = 1, int tracks = 77, int sectors = 26, int secsize = 128, int blksize = 1024, int blktrk = 2, bool readonly = false ) {
      this->path = path;
      this->sides = sides;
      this->tracks = tracks;
      this->sectors = sectors;
      this->secsize = secsize;
      this->blksize = blksize;
      this->blktrk = blktrk;
      this->readonly = readonly;
      this->file.setPath( path );
      this->file.setReadonly( readonly );
      this->track = 0;
      this->sector = 1;
      this->logical = 1;
      Serial.println( "EmuDiskImg constructed ..." );
    }
    void config( String path = "", int sides = 1, int tracks = 77, int sectors = 26, int secsize = 128, int blksize = 1024, int blktrk = 2, bool readonly = false ) {
      this->path = path;
      this->sides = sides;
      this->tracks = tracks;
      this->sectors = sectors;
      this->secsize = secsize;
      this->blksize = blksize;
      this->blktrk = blktrk;
      this->readonly = readonly;
      this->file.setPath( path );
      this->file.setReadonly( readonly );
      this->track = 0;
      this->sector = 1;
      this->logical = 1;
    }
    String getPath() {
      return this->path;
    }
    int translate( int sector, bool reverse = false ) {
      int log2sec[] = {
        1,7,13,19,
        25,5,11,17,
        23,3,9,15,
        21,2,8,14,
        20,26,6,12,
        18,24,4,10,
        16,22
      };
      int sec2log[26];
      for ( int i = 0; i < 26; i++ ) {
        sec2log[ log2sec[ i ] - 1 ] = i + 1;
      }
      if ( reverse ) {
        return sec2log[sector-1];
      } else {
        return log2sec[sector-1];
      }
    }
    std::tuple<int, int, int> trksec( int trk, int log ) {
      int blksecs = this->blksize / this->secsize;
      int ztrk = trk - this->blktrk;
      int zsec = log - 1;
      int abssec = ztrk * this->sectors + zsec;
      int blk = abssec / blksecs;
      int zblksec = abssec % blksecs;
      int blksec = zblksec + 1;
      int sec = this->translate( log );
      return { sec, blk, blksec };
    }
    std::tuple<int, int, int> blksec( int blk, int blksec ) {
      int blksecs = this->blksize / this->secsize;
      int zblksec = blksec - 1;
      int ztrk = ( blk * blksecs + zblksec ) / this->sectors;
      int zsec = ( blk * blksecs + zblksec ) % this->sectors;
      int trk = ztrk + this->blktrk;
      int log = zsec + 1;
      int sec = this->translate( log, true );
      return { trk, log, sec };
    }
    void next() {
      this->logical++;
      if ( this->logical > this->sectors ) {
        this->logical = 1;
        this->track++;
        if ( this->track >= this->tracks ) {
          this->track = 0;
        }
      }
      if ( track < 2 ) {
        this->sector = this->logical;
      } else {
        this->sector = this->translate( this->logical );
      }
    }
    unsigned long pos( int track, int sector ) {
      return track * this->sectors * this->secsize + ( sector - 1 ) * this->secsize;
    }
    void seek( int track, int sector ) {
      this->file.seek( this->pos( track, sector ) );
    }
    String secinfo( int drv, int track = -1, int logical = -1 ) {
      if ( track < 0 ) {
        track = this->track;
      }
      if ( logical < 0 ) {
        logical = this->logical;
      }
      //int sector, blk, blksec;
      auto [ sector, blk, blksec ] = this->trksec( track, logical );
      String info = "";
      info += colors[color].dump_label;
      info += "path: ";
      info += colors[color].dump_value;
      info += this->path;
      info += "\r\n";
      info += colors[color].dump_label;
      info += "drv: ";
      info += colors[color].dump_value;
      info += String( drv );
      info += colors[color].dump_label;
      info += "  trk: ";
      info += colors[color].dump_value;
      info += String( track );
      info += colors[color].dump_label;
      info += "  log: ";
      info += colors[color].dump_value;
      info += String( logical );
      info += colors[color].dump_label;
      info += "  sec: ";
      info += colors[color].dump_value;
      info += String( sector );
      info += "\r\n";
      info += colors[color].dump_label;
      info += "blk: ";
      info += colors[color].dump_value;
      if ( track < this->blktrk ) {
        info += "--:--";
      } else {
        info += String( blk );
        info += ":" + String( blksec );
      }
      info += colors[color].dump_label;
      info += "  op: ";
      info += colors[color].dump_value;
      info += "read";
      info += colors[color].dump_label;
      info += "  size: ";
      info += colors[color].dump_value;
      info += String( this->secsize );
      info += colors[color].dump_label;
      info += "  chksum: ";
      info += colors[color].dump_value;
      info += "---";
      return info;
    }
    void readsec( uint8_t *buffer, int addr, int track, int sector ) {
      this->file.open();
      this->seek( track, sector );
      this->file.read( buffer + addr, this->secsize );
      this->file.close();
      diskActivity();
    }
    void readlog( uint8_t *buffer, int addr, int track = -1, int logical = -1 ) {
      if ( track < 0 ) {
        track = this->track;
      } else {
        this->track = track;
      }
      int sector;
      if ( logical < 0 ) {
        logical = this->logical;
      } else {
        this->logical = logical;
        this->sector = this->translate( logical );
      }
      if ( this->track < 2 ) {
        sector = logical;
      } else {
        sector = this->translate( logical );
      }
      this->readsec( buffer, addr, track, sector );
      diskActivity();
    }
    void writesec( uint8_t *buffer, int addr, int track, int sector ) {
      this->file.open();
      this->seek( track, sector );
      this->file.write( buffer + addr, this->secsize );
      this->file.close();
      diskActivity( true );
    }
    void test() {
      uint8_t data[128];
      EmuFile file = EmuFile( this->path );
      file.open();
      file.read( data, 128 );
      for ( int i = 0; i < 128; i++ ) {
        print( data[i] );
        print( " " );
        if ( ( i % 8 ) == 0 ) {
          println();
        }
      }
      file.close();
    }
};


void setupFs() {
  if (!SD.begin(GPIO_NUM_12, SPI, 40000000)) { // CS pin for Cardputer is GPIO 12
    println( "SD Card failed !!!" );
    return;
  } else {
    uint64_t total = SD.totalBytes() / (1024 * 1024);
    uint64_t used = SD.usedBytes() / (1024 * 1024);
    println( "SD Card Success ..." );
    print( "Total space: " );
    println( total );
    print( "Used space: " );
    println( used );
  }
}

#endif

//
// Ansi C version
//

void disk_rd_activity() {}

void disk_wr_activity() {}

void disk_load_bin( uint8_t *data, size_t size, size_t addr, char *path ) {
  #ifdef ESP32
    File file = SD.open( path, "r" );
    size_t filesize = file.size();
    if ( ( addr + filesize ) < size ) {
      file.seek( 0 );
      file.read( data + addr, size );
      print( "Binary size: " );
      print( filesize );
      print( " loaded at addr: " );
      println( addr );
    } else {
      print( "Binary size: " );
      print( filesize );
      print( " too large to load at addr: " );
      println( addr );
    }
    file.close();
  #else
    FILE *fp;
    fp = fopen( path, "rb" );
    fseek( fp, 0, SEEK_END );
    int filesize = ftell( fp );
    rewind( fp );
    if ( ( addr + filesize ) < size ) {
      fread( data + addr, 1, size, fp );
      print( "Binary size: " );
      print( filesize );
      print( " loaded at addr: " );
      println( addr );
    } else {
      print( "Binary size: " );
      print( filesize );
      print( " too large to load at addr: " );
      println( addr );
    }
    fclose( fp );
  #endif
  disk_rd_activity();
}

//
// cp/m disk img
//

#ifdef ESP32

char *cpm_imgs[] = {
  "/emu/disks/cpm22-1.dsk",
  "/emu/disks/cpm22-2.dsk",
  "/emu/disks/8080tools.cpm",
  "/emu/disks/trek.cpm",
  "", "", "", "",
  "", "", "", "",
  "", "", "", ""
};

#else

char *cpm_imgs[] = {
  "../../sdcard/emu/disks/cpm22-1.dsk",
  "../../sdcard/emu/disks/cpm22-2.dsk",
  "../../sdcard/emu/disks/8080tools.cpm",
  "../../sdcard/emu/disks/trek.cpm",
  "", "", "", "",
  "", "", "", "",
  "", "", "", ""
};

#endif

int cpm_drvs[] = {
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 9, 10, 11,
  12, 13, 14, 15
};

struct Mon_drv {
  int img;
  int trk;
  int log;
};

struct Mon_drv mon_drvs[] = {
  { 0, 0, 1 },
  { 1, 0, 1 }
};

struct Cpm {
  int trks;
  int secs;
  int secsize;
  int trk0;
  int sec0;
  int reserved;
  int exts;
  int extsize;
  int blksecs;
};

struct Cpm cpm = {
  77, 26, 128, 0, 1,
  2, 64, 32, 8
};

int cpm_disk_log2sec_table[] = {
  1,7,13,19,
  25,5,11,17,
  23,3,9,15,
  21,2,8,14,
  20,26,6,12,
  18,24,4,10,
  16,22
};

int cpm_disk_log2sec( int trk, int log ) {
  if ( trk < cpm.reserved ) return log;
  return cpm_disk_log2sec_table[ log - cpm.sec0 ];
}

int cpm_disk_sec2log( int trk, int sec ) {
  if ( trk < cpm.reserved ) return sec;
  int log = 0;
  for ( int i = 0; i < 26; i++ ) {
    if ( cpm_disk_log2sec_table[ i ] == sec ) {
      log = i + cpm.sec0;
      break;
    }
  }
  return log;
}

void cpm_disk_trklog2blksec( int trk, int log, int *blk, int *blksec ) {
  int ztrk = trk - cpm.reserved;
  int zsec = log - 1;
  int abssec = ztrk * cpm.secs + zsec;
  *blk = abssec / cpm.blksecs;
  int zblksec = abssec % cpm.blksecs;
  *blksec = zblksec + 1;
  //int sec = cpm_disk_log2sec( trk, log );
}

void cpm_disk_blksec2trklog( int blk, int blksec, int *trk, int *log ) {
  int zblksec = blksec - 1;
  int ztrk = ( blk * cpm.blksecs + zblksec ) / cpm.secs;
  int zsec = ( blk * cpm.blksecs + zblksec ) % cpm.secs;
  *trk = ztrk + cpm.reserved;
  *log = zsec + 1;
  //int sec = cpm_disk_log2sec( trk, log );
}

bool cpm_disk_isvalid( int trk, int sec ) {
  bool res = ( trk >= cpm.trk0 );
  res = res & ( trk < cpm.trks + cpm.trk0 );
  res = res & ( sec >= cpm.sec0 );
  res = res & ( sec < cpm.secs + cpm.sec0 );
  return res;
}

long int cpm_disk_pos( int trk, int sec ) {
  if ( debug_disk ) {
    print( "cpm_disk_pos cpm.secs: " );
    print( cpm.secs );
    print( " cpm.sec0: " );
    print( cpm.sec0 );
    print( " cpm.secsize: " );
    print( cpm.secsize );
    print( " trk: " );
    print( trk );
    print( " sec: " );
    println( sec );
  }
  long int pos = trk * cpm.secs * cpm.secsize + ( sec - cpm.sec0 ) * cpm.secsize;
  return pos;
}

void mon_drv_next( int drv ) {
  mon_drvs[ drv ].log++;
  if ( mon_drvs[ drv ].log > cpm.secs ) {
    mon_drvs[ drv ].log = cpm.sec0;
    mon_drvs[ drv ].trk++;
    if ( mon_drvs[ drv ].trk > cpm.trks ) {
      mon_drvs[ drv ].trk = cpm.trk0;
    }
  }
}

void print_cpm_disk_sec_info( int drv, int trk, int log ) {
  if ( trk < 0 ) {
    trk = mon_drvs[ drv ].trk;
  }
  if ( log < 0 ) {
    log = mon_drvs[ drv ].log;
  }
  int sec = cpm_disk_log2sec( trk, log );
  int blk, blksec;
  cpm_disk_trklog2blksec( trk, log, &blk, &blksec );
  int img = mon_drvs[ drv ].img;
  char *path = cpm_imgs[ img ];
  printclr( colors[color].dump_label );
  print( "path: " );
  printclr( colors[color].dump_value );
  print( path );
  println( "" );
  printclr( colors[color].dump_label );
  print( "drv: " );
  printclr( colors[color].dump_value );
  print( drv );
  printclr( colors[color].dump_label );
  print( "  trk: " );
  printclr( colors[color].dump_value );
  print( trk );
  printclr( colors[color].dump_label );
  print( "  log: " );
  printclr( colors[color].dump_value );
  print( log );
  printclr( colors[color].dump_label );
  print( "  sec: " );
  printclr( colors[color].dump_value );
  print( sec );
  println( "" );
  printclr( colors[color].dump_label );
  print( "blk: " );
  printclr( colors[color].dump_value );
  if ( trk < cpm.reserved ) {
    print( "--:--" );
  } else {
    print( blk );
    print( ":" );
    print( blksec );
  }
  printclr( colors[color].dump_label );
  print( "  op: " );
  printclr( colors[color].dump_value );
  print( "read" );
  printclr( colors[color].dump_label );
  print( "  size: " );
  printclr( colors[color].dump_value );
  print( cpm.secsize );
  printclr( colors[color].dump_label );
  print( "  chksum: " );
  printclr( colors[color].dump_value );
  print( "---" );
  println( "" );
}

void cpm_disk_rw( bool write, bool mon, int drv, uint8_t *data, int addr, int trk, int sec ) {
  if ( debug_disk ) {
    print( "disk_sec_rw beg: ");
    println( cpm.sec0 );
  }
  bool error = false;
  long int pos = 0;
  int img;
  if ( mon ) {
    img = mon_drvs[ drv ].img;
  } else {
    img = cpm_drvs[ drv ];
  }
  if ( debug_disk ) {
    println( cpm.sec0 );
  }
  char *path = cpm_imgs[ img ];
  if ( cpm_disk_isvalid( trk, sec ) ) {
    #ifdef ESP32
      File file;
      if ( write ) {
        file = SD.open( path, "r+w" );
      } else {
        file = SD.open( path, "r" );
      }
      file.seek( cpm_disk_pos( trk, sec ) );
      if ( write ) {
        file.write( data + addr, 128 );
        disk_wr_activity();
      } else {
        file.read( data + addr, 128 );
        disk_rd_activity();
      }
      file.close();
    #else
      FILE *fp;
      if ( write ) {
        fp = fopen( path, "wb" );
      } else {
        fp = fopen( path, "rb" );
      }
      size_t res;
      pos = cpm_disk_pos( trk, sec );
      fseek( fp, pos, SEEK_SET );
      if ( write ) {
        res = fwrite( data + addr, 1, 128, fp );
      } else {
        res = fread( data + addr, 1, 128, fp );
      }
      if ( res < 128 ) {
        if ( feof( fp ) ) {
          println( "end of file reached ..." );
          error = true;
        } else if ( ferror( fp ) ) {
          println( "file error ..." );
          error = true;
        }
      }
      fclose( fp );
    #endif
  } else {
    println( "cpm_disk_rw invalid trk/sec error ..." );
    error = true;
  }
  if ( error ) {
    print( "addr: " );
    print( addr );
    print( " trk: " );
    print( trk );
    print( " sec: " );
    print( sec );
    println( "" );
  }
  if ( debug_disk ) {
    print( "disk_sec_rw pos: " );
    print( pos );
    print( " addr: " );
    print( addr );
    print( " trk: " );
    print( trk );
    print( " sec: " );
    print( sec );
    println( "" );
    print_hex_lines( addr, data, addr, 8, 16 );
  }
  if ( debug_disk ) {
    print( "disk_sec_rw end: ");
    println( cpm.sec0 );
  }
}

void cpm_disk_rd_log( int drv, uint8_t *data, int addr, int trk, int log ) {
  if ( trk ==  -1 ) {
    trk = mon_drvs[ drv ].trk;
  } else {
    mon_drvs[ drv ].trk = trk;
  }
  if ( log == -1 ) {
    log = mon_drvs[ drv ].log;
  } else {
    mon_drvs[ drv ].log = log;
  }
  int sec = cpm_disk_log2sec( trk, log );
  if ( cpm_disk_isvalid( trk, sec ) ) {
    cpm_disk_rw( false, true, drv, data, addr, trk, sec );
    mon_drv_next( drv );
  }
}

void cpm_disk_rd_sec( int drv, uint8_t *data, int addr, int trk, int sec ) {
  if ( cpm_disk_isvalid( trk, sec ) ) {
    cpm_disk_rw( false, false, drv, data, addr, trk, sec );
    mon_drv_next( drv );
  }
}

void cpm_disk_wr_sec( int drv, uint8_t *data, int addr, int trk, int sec ) {
  if ( cpm_disk_isvalid( trk, sec ) ) {
    cpm_disk_rw( true, false, drv, data, addr, trk, sec );
    mon_drv_next( drv );
  }
}
