
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void doPrompt() {
  print( colors[color].prompt );
  print( ">>> " );
  print( colors[color].reset );
}

void cliSetup() {
  println();
  println( "Monitor" );
  doPrompt();
}


//
// cmd line loop
//

//string status();




//
// line editing loop
//
// This loop performs basic line editing functions such as backspace, left/right cursor movement and enter to
// submit the command.  Enter will send cmd buffer to the cmdLine loop function.
//

const int cmdBufferLen = 101;
char cmd[cmdBufferLen];
int cmdPos = 0;
int cmdEnd = 0;

void cursorLeft() {
  Serial.print( "\x1b[D" );
}

void cursorRight() {
  Serial.print( "\x1b[C" );
}

void cursorSave() {
  Serial.print( "\x1b[s" );
}

void cursorUnsave() {
  Serial.print( "\x1b[u" );
}

void delStrChar( char* str, int index ) {
  int len = strlen( cmd );
  if ( index >= 0 && index < len ) {
    for ( int i = index; i < len; i++ ) {
      str[i] = str[i+1];
    }
  }
  str[len-1] = '\0';
}

void insStrChar( char* str, int index, char ch ) {
  int len = strlen( cmd );
  if ( ( len + 2 ) < cmdBufferLen && index < ( cmdBufferLen - 2 ) ) {
    for ( int i = len; i >= index; i-- ) {
      str[i+1] = str[i];
    }
    str[index] = ch;
  }
}

void lineEdit( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "0d" ) {  // Enter
    println();
    //Serial.println( strlen( cmd ) );
    cmdLine( cmd );
    cmdPos = 0;
    cmd[cmdPos] = '\0';
    doPrompt();
  } else if ( hexStr == "7f" ) {  // Backspace
    if ( cmdPos > 0 ) {
      //String right = cmd.
      cmdPos--;
      delStrChar( cmd, cmdPos );
      cursorLeft();
      cursorSave();
      Serial.print( cmd + cmdPos );
      Serial.print( " " );
      cursorUnsave();
    }
  } else if ( hexStr == "1b5b44" ) {  // Left Arrow
    if ( cmdPos > 0 ) {
      cmdPos--;
      cursorLeft();
    }
  } else if ( hexStr == "1b5b43" ) {  // Right Arrow
    if ( cmdPos < ( strlen( cmd ) ) ) {
      cmdPos++;
      cursorRight();
    }
  } else {  // Printable character
    if ( len == 1 ) {
      if ( cc >= 0x20 && cc <= 0x7e ) {
        if ( cmdPos > ( cmdBufferLen - 3 ) ) {
          cmdPos = cmdBufferLen - 3;
          cursorLeft();
        }
        if ( strlen( cmd ) < ( cmdBufferLen - 2 ) ) {
          insStrChar( cmd, cmdPos++, ch );
          Serial.print( ch );
          cursorSave();
          Serial.print( cmd + cmdPos );
          cursorUnsave();
        }
      }
    }
  }
}

//
// A loop to receive hex data
//
// to be implemented
//

unsigned long rcvHexBeg = 0;
unsigned long rcvHexTimeout = 10000000;

int rcvHexCount = 0;

bool rcvHex = false;

void rcvHexLoop( char ch ) {
  if ( micros() > ( rcvHexBeg + rcvHexTimeout ) ) {
    rcvHex = false;
    Serial.println();
    Serial.print( "Receive Hex data timeout ..." );
  } else if ( ch == '.' ) {
    rcvHex = false;
    Serial.println();
    Serial.print( "Hex Bytes received: " );
    Serial.println( rcvHexCount / 2 );
  } else {
    rcvHexBeg = micros();
    if ( ( rcvHexCount++ % 64 ) == 0 ) {
      Serial.println();
    }
    Serial.print( ch );
  }
}

//
// io redirection and cpu control cmd loop
// This loop is used to perform basic io redirection, cpu control and toggle debugging functions on/off.
// All other characters are forwarded to the lineEdit loop.
//

bool io2cli = true;


void ctrlLoop( int len, char ch, int cc, String hexStr ) {
  if ( hexStr == "1b5b357e" ) {         // PageUp
    println();
    println( "io directed to cpu ..." );
    print( colors[color].cpm_color );
    io2cli = false;
  } else if ( hexStr == "1b5b367e" ) {  // PageDown
    println();
    println( "io directed to cli ..." );
    print( colors[color].reset );
    io2cli = true;
  } else if ( hexStr == "1b5b347e" ) {
    Serial.println( "Cpu running: false" );
    running = false;
  } else if ( hexStr == "1b5b317e" ) {
    Serial.println( "Cpu running: true" );
    running = true;
  } else if ( hexStr == "1b5b34333231307e" || hexStr == "18" ) {  // 4 3 2 1 0 switch to hex mode
    rcvHex = true;
    Serial.println( "Waiting for Hex data ..." );
    rcvHexBeg = micros();
  } else if ( hexStr == "13" ) {
    showState = ! showState;
  } else if ( hexStr == "06" ) {
    fast = ! fast;
    delayTime = 500;
    if ( fast ) {
      delayTime = 0;
    }
  } else if ( hexStr == "1a" ) {  // ctrl-z
    showHex = ! showHex;
  } else {
    if ( rcvHex ) {
      rcvHexLoop( ch );
    } else if ( io2cli ) {
      lineEdit( len, ch, cc, hexStr );
    } else {
      if ( queuePos < queueSize ) {
        if ( cc == 3 ) {
          //println( "ctrl-c send to system ..." );
        }
        queue[queuePos++] = ch;
      }
    }
  }
}

//
// ioLoop reads incoming characters from serial.  If it receives the ESC character, it tries to
// see if it is the start of an ansi key definition, such as 1b5b44, left arrow.  If ansi end
// character is received, it sends the entire ansi hex string to the ctrlLoop for further processing.
// Otherwise, it sends a single character, including a lone ESC character, to the crtlLoop for processing.
// I am not sure if this loop correctly captures all ansi keyboard characters, but it does capture the
// ones in use by the cli at this point.
//

void ioLoop() {
  String ansiEnd = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~";
  if ( Serial.available() ) {
    int len = 0;
    char ch;
    int cc;
    String hx;
    String hexStr = "";
    bool ansi = false;
    unsigned long end = 0;
    unsigned long timeout = 10000;
    do {
      ch = Serial.read();
      if ( ch != 0xff ) {
        cc = (int)ch;
        hx = hex2( cc );
        hexStr += hx;
        len++;
        if ( cc == 0x1b ) {
          ansi = true;
          end = micros() + timeout;
        }
        if ( ansiEnd.indexOf( ch ) != -1 ) {
          ansi = false;
        }
      }
      if ( micros() > end ) {
        ansi = false;
      }
    } while ( ansi );
    if ( showHex ) Serial.println( hexStr );
    ctrlLoop( len, ch, cc, hexStr );
  }
}

