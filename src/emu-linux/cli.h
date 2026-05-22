
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void doPrompt() {
  print( colors[color].prompt );
  print( ">>> " );
  print( colors[color].reset );
}

void cliSetup() {
  println( "" );
  println( "Monitor" );
  doPrompt();
}

//
// line editing loop
//
// This loop performs basic line editing functions such as backspace, left/right cursor movement and enter to
// submit the command.  Enter will send cmd buffer to the cmdLine loop function.
//

#define CLI_BUFFER_LEN 101
const int cmdBufferLen = CLI_BUFFER_LEN;
char cmd[ CLI_BUFFER_LEN ];
unsigned int cmdPos = 0;
unsigned int cmdEnd = 0;

void cursorLeft() {
  print( "\x1b[D" );
}

void cursorRight() {
  print( "\x1b[C" );
}

void cursorSave() {
  print( "\x1b[s" );
}

void cursorUnsave() {
  print( "\x1b[u" );
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

void cli_enter() {
  println( "" );
  cmdLine( cmd );
  cmdPos = 0;
  cmd[cmdPos] = '\0';
  doPrompt();
}

void cli_backspace() {
  if ( cmdPos > 0 ) {
    cmdPos--;
    delStrChar( cmd, cmdPos );
    cursorLeft();
    cursorSave();
    print( cmd + cmdPos );
    print( " " );
    cursorUnsave();
  }
}

void cli_leftarrow() {
  if ( cmdPos > 0 ) {
    cmdPos--;
    cursorLeft();
  }
}

void cli_rightarrow() {
  if ( cmdPos < ( strlen( cmd ) ) ) {
    cmdPos++;
    cursorRight();
  }
}

void cli_default( int len, char ch, int cc, char *hexStr ) {
  (void)hexStr;
  if ( len == 1 ) {
    if ( cc >= 0x20 && cc <= 0x7e ) {
      if ( cmdPos > ( cmdBufferLen - 3 ) ) {
        cmdPos = cmdBufferLen - 3;
        cursorLeft();
      }
      if ( strlen( cmd ) < ( cmdBufferLen - 2 ) ) {
        insStrChar( cmd, cmdPos++, ch );
        char buffer[2];
        buffer[0] = ch;
        buffer[1] = '\0';
        print( buffer ); // why, because the char gets mistaken for an integer when printing
        cursorSave();
        print( cmd + cmdPos );
        cursorUnsave();
      }
    }
  }
}

cmd_entry_t cmds_cli[] = {
  { "0a", cli_enter, "", "Enter" }, // linux
  { "0d", cli_enter, "", "Enter" }, // arduino
  { "7f", cli_backspace, "", "Backspace" },
  { "1b5b44", cli_leftarrow, "", "Left Arrow" },
  { "1b5b43", cli_rightarrow, "", "Right Arrow" },
  { NULL, NULL, NULL, NULL }
};

static bool cli_exec( cmd_entry_t *cmds, char *cmd ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    if ( strcmp( cmds[i].name, cmd ) == 0 ) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

void lineEdit( int len, char ch, int cc, char *hexStr ) {
  bool res = false;
  res = cli_exec( cmds_cli, hexStr );
  if ( ! res ) {
    cli_default( len, ch, cc, hexStr );
  }
}
