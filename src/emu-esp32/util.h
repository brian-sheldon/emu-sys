
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int dec2int( char *str ) {
  char *endptr;
  int v = strtol( str, &endptr, 10 );
  if ( str == endptr ) {
    return 0;
  } else {
    return v;
  }
}

int hex2int( char *str ) {
  char *endptr;
  int v = strtol( str, &endptr, 16 );
  if ( str == endptr ) {
    return 0;
  } else {
    return v;
  }
}

char *dec0( int v, int width ) {
  static char buffer[ 40 ];
  snprintf( buffer, sizeof( buffer ), "%*d", width, v );
  return buffer;
}

char *hex0( int v, int width ) {
  static char buffer[ 20 ];
  snprintf( buffer, sizeof( buffer ), "%0*x", width, v );
  return buffer;
}

char *hex2( int v ) {
  return hex0( v, 2 );
}

char *hex4( int v ) {
  return hex0( v, 4 );
}

char *ascLine( uint8_t *data, int pos, int cols ) {
  static char line[17];
  line[0] = '\0';
  if ( cols <= 16 ) {
    for ( int i = 0; i < cols; i++ ) {
      char ch = data[pos+i];
      int cc = (int)ch;
      if ( cc >= 0x20 && cc <= 0x7e ) {
        line[i] = ch;
      } else {
        line[i] = '.';
      }
    }
    line[ cols ] = '\0';
  }
  return line;
}

char *hexLine( uint8_t *data, int pos, int cols ) {
  static char line[50];
  line[0] = '\0';
  if ( cols <= 16 ) {
    for ( int i = 0; i < cols; i++ ) {
      strcat( line, hex2( data[pos+i] ) );
      if ( ( i % 2 ) == 1 ) strcat( line, " " );
    }
  }
  return line;
}

char *hexLines( int addr, uint8_t *data, int pos, int rows, int cols ) {
  char lf[] = "\r\n";
  static char lines[1500];
  lines[0] = '\0';
  for ( int row = 0; row < rows; row++ ) {
    if ( row != 0 ) strcat( lines, lf );
    strcat( lines, colors[color].dump_addr );
    strcat( lines, hex4( addr ) );
    strcat( lines, " " );
    strcat( lines, colors[color].dump_hex );
    strcat( lines, hexLine( data, pos, cols ) );
    //strcat( lines, "" );
    strcat( lines, colors[color].dump_asc );
    strcat( lines, ascLine( data, pos, cols ) );
    addr += cols;
    pos += cols;
  }
  return lines;
}

void displayRuler() {
  print( colors[color].ruler_color );
  int tens = 1;
  for ( int i = 1; i <= rulerColumns; i++ ) {
    if ( ( i % 10 ) == 0 ) {
      print( dec0( tens++, 0 ) );
    } else if ( ( i % 5 ) == 0 ) {
      print( "+" );
    } else {
      print( "." );
    }
  }
  println( "" );
}

#ifdef ARDUINO_M5STACK_CARDPUTER

int utilLoopMillis = 0;

void displayBlink( int x, int color ) {
  M5Cardputer.Display.fillCircle( x, 15, 10, color );
  utilLoopMillis = millis();
}

void diskActivity( bool write = false ) {
  if ( write ) {
    displayBlink( 225, RED );
  } else {
    displayBlink( 200, GREEN );
  }
}


void utilLoop() {
  if ( millis() > utilLoopMillis + 250 ) {
    M5Cardputer.Display.fillCircle( 200, 15, 10, BLACK );
    M5Cardputer.Display.fillCircle( 225, 15, 10, BLACK );
    utilLoopMillis = millis();
  }
}

#endif
