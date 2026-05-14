
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <SPI.h>
#include <SD.h>
#include <FS.h>

#include <tuple>

//#define SD_SPI_SCK_PIN  40
//#define SD_SPI_MISO_PIN 39
//#define SD_SPI_MOSI_PIN 14
//#define SD_SPI_CS_PIN   12

// SD Card Initialization
//SPI.begin( SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN );

void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);

void printf_log(const char *format, ...);
void println_log(const char *str);

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
      dir.close();
    }
    void mkdir( String path ) {
      SD.mkdir( path );
    }
    void rmdir( String path ) {
      SD.rmdir( path );
    }
    void mv( String srcpath, String dstpath ) {
      //
    }
    void rm( String path ) {
      SD.remove( path );
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
        return bytes;
      } else {
        return 0;
      }
    }
    void write( uint8_t *data, size_t size ) {
      if ( this->file ) {
        this->file.write( data, size );
        this->file.flush();
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
    }
    void writesec( uint8_t *buffer, int addr, int track, int sector ) {
      this->file.open();
      this->seek( track, sector );
      this->file.write( buffer + addr, this->secsize );
      this->file.close();
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
    println( "SD Card Success ..." );
    print( "Total space: " );
    println( SD.totalBytes() / (1024 * 1024) );
    print( "Used space: " );
  }
}

/*

void createDir(fs::FS &fs, const char *path) {
  printf_log("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    println_log("Dir created");
  } else {
    println_log("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path) {
  printf_log("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    println_log("Dir removed");
  } else {
    println_log("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path) {
  printf_log("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    println_log("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  printf_log("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    println_log("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    println_log("File written");
  } else {
    println_log("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  printf_log("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    println_log("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    println_log("Message appended");
  } else {
    println_log("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  printf_log("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    println_log("File renamed");
  } else {
    println_log("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  printf_log("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    println_log("File deleted");
  } else {
    println_log("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    printf_log("%u bytes read for %lu ms\n", flen, end);
    file.close();
  } else {
    println_log("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file) {
    println_log("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  printf_log("%u bytes written for %lu ms\n", 2048 * 512, end);
  file.close();
}

void printf_log( const char *format, ... ) {
  char buf[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, 256, format, args);
  va_end(args);
  Serial.print(buf);
  //canvas.printf(buf);
  //canvas.pushSprite(0, 0);
}

void println_log(const char *str) {
  Serial.println(str);
  //canvas.println(str);
  //canvas.pushSprite(0, 0);
}
*/
