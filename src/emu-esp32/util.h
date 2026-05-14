
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdlib.h>

int printVPos = 0;
bool printInEsc = false;
void do_print( const char *str ) {
  Serial.print( str );
  /*
  char buffer[1500];
  printInEsc = false;
  int j = 0;
  for ( int i = 0; i < 1499; i++ ) {
    char ch = str[i];
    if ( ch == '\0' ) {
      break;
    }
    if ( printInEsc ) {
      if ( ch == 'm' ) {
        printInEsc = false;
      }
    } else {
      if ( ch == '\x1b' ) {
        printInEsc = true;
      }
      if ( ch == '\n' ) {
        buffer[j] = '\0';
        j = 0;
        M5Cardputer.Display.println( buffer );
      } else {
        buffer[j++] = ch;
      }
    }
  }
  buffer[j] = '\0';
  M5Cardputer.Display.print( buffer );
  */
}

void print( String str ) {
  do_print( str.c_str() );
}

void print( const char *str ) {
  //printf( "%s", str );
  do_print( str );
}

void print( char *str ) {
  const char* cstr = str;
  do_print( str );
}

void print( int v ) {
  //printf( "%d", v );
  char buffer[20];
  itoa( v, buffer, sizeof( buffer ) );
  const char* cstr = buffer;
  do_print( buffer );
}

void println() {
  print( "\r\n" );
}

void println( String str ) {
  print( str );
  println();
}

void println( const char *str ) {
  print( str );
  println();
}

void println( char *str ) {
  print( str );
  println();
}

void println( int v ) {
  print( v );
  println();
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

char *dec0( int v, int width = 0 ) {
  static char buffer[ 40 ];
  snprintf( buffer, sizeof( buffer ), "%*d", width, v );
  return buffer;
}

char *hex0( int v, int width = 0 ) {
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
      print( dec0( tens++ ) );
    } else if ( ( i % 5 ) == 0 ) {
      print( "+" );
    } else {
      print( "." );
    }
  }
  println();
}

int utilLoopMillis = 0;

void displayBlink( int color ) {
  M5Cardputer.Display.fillCircle( 210, 30, 20, color );
  utilLoopMillis = millis();
}

void diskActivity( bool write = false ) {
  if ( write ) {
    displayBlink( RED );
  } else {
    displayBlink( GREEN );
  }
}


void utilLoop() {
  if ( millis() > utilLoopMillis + 250 ) {
    M5Cardputer.Display.fillCircle( 210, 30, 20, BLACK );
    utilLoopMillis = millis();
  }
}
