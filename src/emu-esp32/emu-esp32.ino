
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "M5Cardputer.h"

#include "cfg.h"
#include "util.h"
#include "state.h"
#include "cmd.main.h"
#include "cli.h"

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  
  M5Cardputer.Display.setRotation(1); // Landscape
  M5Cardputer.Display.fillScreen( BLACK );
  M5Cardputer.Display.setCursor( 5, 5 );
  M5Cardputer.Display.setTextColor( BLUE );
  M5Cardputer.Display.setTextSize( 6 );
  //M5Cardputer.Display.setTextScroll(true);
  M5.Display.println( "CP/M" );

  Serial.begin( 115200 );
  delay( 2000 );
  
  Serial.println( "Starting ..." );
  
  setupFs();

  setupEmu();
  
  setupCmd();

  cliSetup();
}

void loop() {
  M5Cardputer.update();
  utilLoop();
  ioLoop();
  //loopEmu();
  cpu_frame();
  delay( 1 );
}

