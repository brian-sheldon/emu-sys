
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

void util_delay( long ms ) {
  #ifdef ESP32
    delay( ms );
  #else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = ( ms % 1000 ) * 1000000L;
    nanosleep( &ts, NULL );
  #endif
}

//
// Allows the default radix to be set as there are cases where it makes
// sense to have the default radix be 16 to avoid having to constantly
// use prefixes.  In order to still allow decimal value entry, the '#'
// has been added as a prefix for force the radix to be decimal.
//
// Signs are also supported and are usually always the first character.
// However, as it is also common to prefix numbers with the '#' sign as is
// common in assemblers, this parser allows the sign to be used immediately
// after the '#'.  But due to the dual nature of the use fo teh '#' in this
// parser, the '#' can also come after the sign so as to be consistent with
// the other prefixes that are used for a non-base 10 radix.
//
// So the following are examples of the number syntaxes are allowed.
//
// 1920 - would be whatever the default radix is
// #1920 = radix 10, eec
// $1920 = radix 16, hex
// 0x1920 - radix 16, hex
// %11001 - radix 2, binary
// 0b11001 - radix 2, binary
// -0x02bf - radix 16, negative hex num
// 0o275 - radix 8, octal
// #0xfc - radix 16, hex
// #-0xfc = radix 16, necative hex num
// -#128 = radix 10, negative dec num, nto a standard format
// #-128 - same as above
//

long pnum( char *word, int radix ) {
  int len = strlen( word );
  char sign = '+';
  long val = 0;
  int pos = 0;
  if ( pos < len && word[ pos ] == '#' ) {
    radix = 10;
    pos++;
  }
  if ( pos < len && ( word[ pos ] == '-' || word[ pos ] == '+' ) ) {
    sign = word[ pos ];
    pos++;
  }
  if ( pos < len ) {
    if ( word[ pos ] == '$' ) {
      radix = 16;
      pos++;
    } else if ( word[ pos ] == '#' ) {
      radix = 10;
      pos++;
    } else if ( word[ pos ] == '%' ) {
      radix = 2;
      pos++;
    } else if ( word[ pos ] == '0' ) {
      pos++;
      // many legacy systems assumed a leading 0 inticated octal
      // not recommended as it is common to enter hex with leading 0s to keep consistent length
      // uncommenting the following line will use the legacy way
      //radix = 8;
      if ( pos < len ) {
        if ( word[ pos ] == 'x' || word[ pos ] == 'X' ) {
          radix = 16;
          pos++;
        } else if ( word[ pos ] == 'b' || word[ pos ] == 'B' ) {
          radix = 2;
          pos++;
        } else if ( word[ pos ] == 'o' || word[ pos ] == 'O' || word[ pos ] == 'q' || word[ pos] == 'Q' ) {
          radix = 8;
          pos++;
        }
      } else {
        pos--; // for when a lone zero is entered
      }
    }
  }
  char *endptr;
  val = strtol( word + pos, &endptr, radix );
  bool success = true;
  if ( ( word + pos ) == endptr ) {
    println( "error not a number ..." );
    success = false;
  } else if ( ( word + len ) == endptr ) {
    success = true;
  } else {
    println( "error non-number chars found ..." );
    success = false;
  }
  if ( success ) {
    if ( sign == '-' ) {
      val = -val;
    }
  } else {
    val = 0;
  }
  //printf( "word: %s radix: %d sign: %c success: %d rem: %ld\n", word, radix, sign, success, val );
  return val;
}

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

void print_hex_lines( int addr, uint8_t *data, int pos, int rows, int cols ) {
  for ( int row = 0; row < rows; row++ ) {
    printclr( colors[color].dump_addr );
    print( hex4( addr ) );
    print( " " );
    printclr( colors[color].dump_hex );
    print( hexLine( data, pos, cols ) );
    printclr( colors[color].dump_asc );
    print( ascLine( data, pos, cols ) );
    println( "" );
    addr += cols;
    pos += cols;
  }
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
