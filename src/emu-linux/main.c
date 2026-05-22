
#include <stdio.h>

#include "out.h"
#include "cfg.h"
#include "util.h"

#include "state.h"
//#include "cmd.h"
#include "cmd.main.h"
#include "cli.h"
#include "ctrl.h"
#include "io.h"

//#include "emu.h"

void setup() {
  initTermios(0);
  //setupFs();
  setupEmu();
  setupCmd();
  cliSetup();
}

void loop() {
  ioLoop();
  cpu_frame();
}

int main() {
  
  setup();

  while ( true ) {
    loop();
  }

  resetTermios();

  return 0;
}



