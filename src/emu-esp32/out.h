
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

int printVPos = 0;
bool printInEsc = false;
void do_print( const char *str ) {
  #ifdef ARDUINO_M5STACK_CARDPUTER
  Serial.print( str );
  #else
  printf( "%s", str );
  #endif
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

//void print( char ch ) {
  //char buffer[2];
  //snprintf( buffer, sizeof( buffer ), "%c", ch );
  //do_print( buffer );
//}

void print( int v ) {
  //printf( "%d", v );
  char buffer[100];
  snprintf( buffer, sizeof( buffer ), "%d", v );
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

