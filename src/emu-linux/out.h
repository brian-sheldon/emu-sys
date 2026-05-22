
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#define print(x) _Generic((x), \
  char*: print_cstr,           \
  bool: print_int,              \
  int: print_int,              \
  uint16_t: print_int,              \
  unsigned long: print_int,    \
  float: print_float,         \
  double: print_float,         \
  default: print_none          \
)(x)

void print_cstr( char *str ) {
  printf( "%s", str );
  fflush( stdout );
}

void print_int( int v ) {
  printf( "%d", v );
  fflush( stdout );
}

void print_float( float v ) {
  printf( "%.2f", v );
  fflush( stdout );
}

void print_none( char *str ) {
  print_cstr( str );
}

//

#define println(x) _Generic((x), \
  char*: println_cstr,           \
  bool: println_int,              \
  int: println_int,              \
  uint16_t: println_int,              \
  unsigned long: println_int,    \
  float: println_float,         \
  double: println_float,         \
  default: println_none          \
)(x)

void println_cstr( char *str ) {
  printf( "%s\n", str );
}

void println_int( int v ) {
  printf( "%d\n", v );
}

void println_float( float v ) {
  printf( "%.2f\n", v );
}

void println_none( char *str ) {
  println_cstr( str );
}

void printclr( char *clr ) {
  print_cstr( clr );
}

