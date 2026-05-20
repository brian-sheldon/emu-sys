
#include "./io.h"

int main() {
  initTermios( 0 );
  while ( true ) {
    loop();
  }
  resetTermios();
  return 0;
}

