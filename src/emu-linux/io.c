
#include "./io.h"

int main() {
  initTermios( 0 );
  while ( true ) {
    ioLoop();
  }
  resetTermios();
  return 0;
}

