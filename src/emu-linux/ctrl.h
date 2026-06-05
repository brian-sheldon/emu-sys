
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

//
// io redirection and cpu control cmd loop
// This loop is used to perform basic io redirection, cpu control and toggle debugging functions on/off.
// All other characters are forwarded to the lineEdit loop.
//

bool io2cli = true;
bool show_hex = false;
bool backspace2del = true;
bool io_key_map = true;

void ctrl_f12() {
  io2cli = ! io2cli;
}

void ctrl_f10() {
  io_key_map = ! io_key_map;
}

void ctrl_f9() {
  backspace2del = ! backspace2del;
}

void ctrl_del() {
  
}

void ctrl_pageup() {
  println( "" );
  println( "io directed to cpu ..." );
  print( colors[color].cpm_color );
  io2cli = false;
}

void ctrl_pagedown() {
  println( "" );
  println( "io directed to cli ..." );
  print( colors[color].reset );
  io2cli = true;
}

void ctrl_end() {
  cpuState.running = true;
  cpuState.on = true;
  println( "cpu: on" );
}

void ctrl_home() {
  cpuState.running = false;
  cpuState.on = false;
  println( "cpu: off" );
}

void ctrl_1d() {
  show_hex = ! show_hex;
}

cmd_entry_t cmds_ctrl[] = {
  { "1b5b32347e", ctrl_f12, "F12", "Toggle io between monitor or system" },
  { "1b5b32307e", ctrl_f9, "F9", "Toggle backspace 7f - 08" },
  { "1b5b32317e", ctrl_f10, "F10", "keymap toggle" },
  //{ "1b5b357e", ctrl_pageup, "PageUp", "connect io to system" },
  //{ "1b5b367e", ctrl_pagedown, "PageDown", "connect io to monitor" },
  //{ "1b5b46", ctrl_end, "End", "turn cpu on" },
  //{ "1b5b347e", ctrl_end, "End", "turn cpu on" },
  //{ "1b5b48", ctrl_home, "Home", "turn cpu off" },
  //{ "1b5b317e", ctrl_home, "Home", "turn cpu off" },
  { "1d", ctrl_1d, "ctrl-]", "Toggle show keys as hex" },
  { NULL, NULL, NULL, NULL }
};

static bool ctrl_exec( cmd_entry_t *cmds, char *cmd ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    if ( strcmp( cmds[i].name, cmd ) == 0 ) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

static int ctrl_keymap( char *hexstr ) {
  for ( int i = 0; keymap[i].src != NULL; i++ ) {
    if ( strcmp( keymap[i].src, hexstr ) == 0 ) {
      if ( strcmp( keymap[i].dst, "" ) != 0 ) {
        return i;
      }
    }
  }
  return -1;
}

void ctrlLoop( int len, char ch, int cc, char *hexStr ) {
  bool res = false;
  if ( io_key_map != 0 ) {
    int mapindex = ctrl_keymap( hexStr );
    if ( mapindex >= 0 ) {
      hexStr = keymap[mapindex].dst;
      if ( strlen( hexStr ) == 2 ) {
        cc = (int)strtol( hexStr, NULL, 16 );
        ch = (char)cc;
      }
    }
  }
  res = ctrl_exec( cmds_ctrl, hexStr );
  if ( ! res ) {
    if ( show_hex ) {
      println( hexStr );
    } else {
      if ( io2cli ) {
        lineEdit( len, ch, cc, hexStr );
      } else {
        if ( queuePos < queueSize ) {
          if ( cc == 0x7f ) {
            if ( backspace2del ) {
              cc = 0x08;
              ch = (char)cc;
            }
          }
          #ifndef ARDUINO
            if ( ch == 0x0a ) {
              ch = 0x0d;
            }
          #endif
          queue[queuePos++] = ch;

        }
      }
    }
  }
}


