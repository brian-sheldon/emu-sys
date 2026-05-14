
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
  
  // 240 x 135
  M5Cardputer.Display.setRotation(1); // Landscape
  M5Cardputer.Display.fillScreen( BLACK );
  M5Cardputer.Display.setCursor( 5, 5 );
  M5Cardputer.Display.setTextColor( BLUE );
  M5Cardputer.Display.setTextSize( 6 );
  //M5Cardputer.Display.setTextScroll(true);
  M5.Display.println( "CP/M" );
  M5Cardputer.Display.setTextSize( 2 );
  M5Cardputer.Display.setCursor( 2, 100 );

  Serial.begin( 115200 );
  delay( 2000 );
  
  Serial.println( "Starting ..." );
  
  setupFs();

  setupEmu();
  
  setupCmd();

  cliSetup();
}

String keyData = "";
void loop() {
  M5Cardputer.update();
  utilLoop();
  ioLoop();
  //loopEmu();
  cpu_frame();
  if ( M5Cardputer.Keyboard.isChange() ) {
    if ( M5Cardputer.Keyboard.isPressed() ) {
      String nowData = "";
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
      for (auto i : status.word) {
        nowData += i;
      }
      keyData += nowData;
      if (status.del) {
        if ( keyData.length() > 2 ) keyData.remove( keyData.length() - 1 );
        cursorLeft();
        cursorSave();
        print( " " );
        cursorUnsave();
      }
      if ( status.enter ) {
        println();
        cmdLine( keyData );
        doPrompt();
        M5Cardputer.Display.fillRect( 2, 100, 135, 240, BLACK );
        M5.Display.print( "          " );
        M5Cardputer.Display.setCursor( 2, 100 );
        M5.Display.print( ">>> " );
        keyData = "";
      }
      print( nowData );
      M5.Display.print( nowData );
      //keyData = "";
    }
  }
  delay( 1 );
}

