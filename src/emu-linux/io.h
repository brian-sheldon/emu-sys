
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
//#include <iostream>
#include <string.h>
#include <time.h>

static struct termios old, new1;
void initTermios( int echo ) {
  tcgetattr( 0, &old );
  new1 = old;
  new1.c_lflag &= ~ICANON;
  new1.c_lflag &= echo ? ECHO : ~ECHO;
  new1.c_cc[VMIN] = 0;
  new1.c_cc[VTIME] = 0;
  tcsetattr( 0, TCSANOW, &new1 );
}

void resetTermios( void ) {
  tcsetattr( 0, TCSANOW, &old );
}

void loop() {
  const char ansiEnd[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~";
  int len = 0;
  int num = 0;
  char buffer[20];
  char ch;
  int cc;
  char hx[3];
  char hexStr[20] = "";
  bool ansi = false;
  unsigned long end = 0;
  unsigned long timeout = 5;
  struct timespec start, now;
  do {
    num = read( 0, &buffer, 1 );
    if ( num > 0 ) {
      cc = buffer[0];
      ch = (char)cc;
      snprintf( hx, sizeof( hx ), "%02x", cc );
      strcat( hexStr, hx );
      len++;
      if ( cc == 0x1b ) {
        clock_gettime( CLOCK_MONOTONIC, &start );
        ansi = true;
      }
      if ( strchr( ansiEnd, ch ) ) {
        ansi = false;
      }
    }
    clock_gettime( CLOCK_MONOTONIC, &now );
    if ( ( now.tv_nsec - start.tv_nsec ) > 20000 ) ansi = false;
  } while ( ansi );
  if ( len > 0 ) {
    printf( "%s\n", hexStr );
  }
}





