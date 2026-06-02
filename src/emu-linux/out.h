
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdio.h>
#include <stdint.h>

#define print(x) _Generic((x), \
  const char*: print_ccstr,     \
  char*: print_cstr,           \
  bool: print_int,              \
  int: print_int,              \
  long int: print_int,              \
  long long: print_int,           \
  uint8_t: print_int,              \
  uint16_t: print_int,              \
  uint32_t: print_int,         \
  uint64_t: print_uint64,         \
  float: print_float,         \
  double: print_float,         \
  default: print_none          \
)(x)

void print_ccstr( const char *str ) {
  printf( "%s", str );
  fflush( stdout );
}

void print_cstr( char *str ) {
  printf( "%s", str );
  fflush( stdout );
}

void print_int( int v ) {
  printf( "%d", v );
  //printf( "%ll", v );
  fflush( stdout );
}

void print_uint64( uint64_t v ) {
  int l = v & 0xffffffff;
  int h = v >> 32;
  printf( "%d %d", h, l );
  //printf( "%" PRIu64 "", v );
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
  long int: println_int,              \
  uint8_t: println_int,              \
  uint16_t: println_int,              \
  uint32_t: println_int,         \
  uint64_t: println_int,         \
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

void printch( char ch ) {
  printf( "%c", ch );
  fflush( stdout );
}

void printlnch( char ch ) {
  printf( "%c\n", ch );
}

void printclr( char *clr ) {
  print_cstr( clr );
}

