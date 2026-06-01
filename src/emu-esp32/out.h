
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

int printVPos = 0;
bool printInEsc = false;
void do_print( const char *str ) {
  #ifdef ARDUINO
    Serial.print( str );
  #endif
  #ifdef __linux__
    printf( "%s", str );
  #endif
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

void printch( char ch ) {
  char buffer[2];
  snprintf( buffer, sizeof( buffer ), "%c", ch );
  do_print( buffer );
}

void print( int v ) {
  //print( "print %d: " );
  char buffer[100];
  snprintf( buffer, sizeof( buffer ), "%d", v );
  do_print( buffer );
}

//void print( double v ) {
  //char buffer[100];
  //snprintf( buffer, sizeof( buffer ), "%.2f", v );
  //do_print( buffer );
//}

void println() {
  print( "\r\n" );
}

void printlnch( char ch ) {
  printch( ch );
  println();
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

//void println( double v ) {
  //print( v );
  //println();
//}

void printclr( char *clr ) {
  print( clr );
}

