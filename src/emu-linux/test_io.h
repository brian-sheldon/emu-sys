
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#ifdef __linux__
  
  #include <stdio.h>
  #include <stdbool.h>
  #include <unistd.h>
  #include <termios.h>
  #include <string.h>
  #include <time.h>

  static struct termios old, new1;
  void initTermios( int echo ) {
    tcgetattr( 0, &old );
    new1 = old;
    new1.c_lflag &= ~( ICANON | IXON );  // add | ISIG to prevent exit signal
    new1.c_lflag &= echo ? ECHO : ~ECHO;
    new1.c_cc[VQUIT] = _POSIX_VDISABLE;
    new1.c_cc[VSTOP] = _POSIX_VDISABLE;
    new1.c_cc[VSTART] = _POSIX_VDISABLE;
    new1.c_cc[VINTR] = 0x1c;  // change exit signal from ctrl-c to ctrl-d
    new1.c_cc[VMIN] = 0;
    new1.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new1 );
  }

  void resetTermios( void ) {
    tcsetattr( 0, TCSANOW, &old );
  }

#endif

//
// ioLoop reads incoming characters from serial.  If it receives the ESC character, it tries to
// see if it is the start of an ansi key definition, such as 1b5b44, left arrow.  If ansi end
// character is received, it sends the entire ansi hex string to the ctrlLoop for further processing.
// Otherwise, it sends a single character, including a lone ESC character, to the crtlLoop for processing.
// I am not sure if this loop correctly captures all ansi keyboard characters, but it does capture the
// ones in use by the cli at this point.
//


void ioLoop() {
  const char ansiEnd[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~";
  #ifdef ARDUINO
    if ( ! Serial.available() ) return;
  #endif
  int len = 0;
  int num = 0;
  char buffer[20];
  char ch;
  int cc;
  char hx[3];
  char hexStr[20] = "";
  bool ansi = false;
  #ifdef ARDUINO
    int start = 0;
    int now = 0;
  #endif
  #ifdef __linux__
    struct timespec start, now;
  #endif
  do {
    #ifdef ARDUINO
      cc = Serial.read();
      if ( cc >= 0 ) {
        num = 1;
      } else {
        num = 0;
      }
    #endif
    #ifdef __linux__
      num = read( 0, &buffer, 1 );
      if ( num > 0 ) {
        cc = buffer[0];
      }
    #endif
    if ( num > 0 ) {
      ch = (char)cc;
      snprintf( hx, sizeof( hx ), "%02x", cc );
      strcat( hexStr, hx );
      len++;
      if ( cc == 0x1b ) {
        #ifdef ARDUINO
          start = micros();
        #endif
        #ifdef __linux__
          clock_gettime( CLOCK_MONOTONIC, &start );
        #endif
        ansi = true;
      }
      if ( strchr( ansiEnd, ch ) ) {
        ansi = false;
      }
    }
    if ( ansi ) {
    #ifdef ARDUINO
      now = micros();
      if ( ( now - start ) > 30000 ) ansi = false;
    #endif
    #ifdef __linux__
      clock_gettime( CLOCK_MONOTONIC, &now );
      if ( ( now.tv_nsec - start.tv_nsec ) > 30000 ) ansi = false;
    #endif
    }
  } while ( ansi );
  if ( len > 0 ) {
    bool show_hex = true;
    if ( show_hex ) printf( "%s\n", hexStr );
    //ctrlLoop( len, ch, cc, hexStr );
  }
}


