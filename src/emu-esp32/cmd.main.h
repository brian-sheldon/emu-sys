
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include "cmd.h"

#include "emu.fs.h"
#include "emu.h"



#include <tuple>

char defcmd[20] = "";

#include "cmd.fs.h"
#include "cmd.disk.h"
#include "cmd.cpu.h"
#include "cmd.trace.h"

static void main_clrcmd() {
  strcpy( defcmd, "" );
}

static void main_ruler() {
  if ( cmdline.plen > 1 ) {
    rulerColumns = dec2int( cmdline.p1 );
  }
  displayRuler();
}

static void main_ruleron() {
  rulerOn = true;
}

static void main_ruleroff() {
  rulerOn = false;
}

static void main_colors() {
  println( "\x1b[0;30mBright: 0 Color: 0" );
  println( "\x1b[0;31mBright: 0 Color: 1" );
  println( "\x1b[0;32mBright: 0 Color: 2" );
  println( "\x1b[0;33mBright: 0 Color: 3" );
  println( "\x1b[0;34mBright: 0 Color: 4" );
  println( "\x1b[0;35mBright: 0 Color: 5" );
  println( "\x1b[0;36mBright: 0 Color: 6" );
  println( "\x1b[0;37mBright: 0 Color: 7" );
  println( "\x1b[1;30mBright: 1 Color: 0" );
  println( "\x1b[1;31mBright: 1 Color: 1" );
  println( "\x1b[1;32mBright: 1 Color: 2" );
  println( "\x1b[1;33mBright: 1 Color: 3" );
  println( "\x1b[1;34mBright: 1 Color: 4" );
  println( "\x1b[1;35mBright: 1 Color: 5" );
  println( "\x1b[1;36mBright: 1 Color: 6" );
  println( "\x1b[1;37mBright: 1 Color: 7" );
}

static void help();
static void words();

cmd_entry_t cmds_main[] = {
  { "clrcmd", main_clrcmd, "", "clears the current cmd repeated when cmdline is blank" },
  { "ruler", main_ruler, "[columns]", "display ruler and set length" },
  { "ruleron", main_ruleron, "", "ruler on for every cmd" },
  { "ruleroff", main_ruleroff, "", "ruler off" },
  { "colors", main_colors, "", "display colors" },
  { "help", help, "", "detailed help for all commands" },
  { "words", words, "", "list all commands" },
  { NULL, NULL, NULL, NULL }
};

static void help_one( char *grp, cmd_entry_t *cmds ) {
  char buffer[80];
  memset( buffer, '=', 5 );
  buffer[5] = '\0';
  print( colors[color].help_bar );
  print( buffer );
  snprintf( buffer, sizeof( buffer ), "  %s%-s  ", colors[color].help_title, grp );
  print( buffer );
  int rem = 79 - 5 - 4 - strlen( grp );
  memset( buffer, '=', rem );
  buffer[rem] = '\0';
  print( colors[color].help_bar );
  println( buffer );
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    snprintf( buffer, sizeof( buffer ), "%s%-8s  %s%-18s ; %s%s", colors[color].help_cmd, cmds[i].name, colors[color].help_params, cmds[i].params, colors[color].help_desc, cmds[i].desc );
    println( buffer );
  }
}

static void help() {
  
  help_one( "cpu", cmds_cpu );
  help_one( "trace", cmds_trace );
  help_one( "disk", cmds_disk );
  help_one( "fs", cmds_fs );
  help_one( "main", cmds_main );
}

static void words_one( cmd_entry_t *cmds ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    print( cmds[i].name );
    print( " " );
  }
}

static void words() {
  print( "[ " );
  words_one( cmds_cpu );
  words_one( cmds_trace );
  words_one( cmds_disk );
  words_one( cmds_fs );
  words_one( cmds_main );
  println( "]" );
}

static bool exec_one( cmd_entry_t *cmds, char *cmd ) {
  for ( int i = 0; cmds[i].name != NULL; i++ ) {
    if ( strcmp( cmds[i].name, cmd ) == 0 ) {
      cmds[i].func();
      return true;
    }
  }
  return false;
}

static void exec( char *cmd ) {
  bool res = false;
  if ( ! res ) res = exec_one( cmds_cpu, cmd );
  if ( ! res ) res = exec_one( cmds_trace, cmd );
  if ( ! res ) res = exec_one( cmds_disk, cmd );
  if ( ! res ) res = exec_one( cmds_fs, cmd );
  if ( ! res ) res = exec_one( cmds_main, cmd );
  if ( ! res ) {
    if ( strcmp( cmd, "" ) ) {
      print( "[" );
      print( cmd );
      println( "] not recognized ..." );
    }
  }
}

void do_cmd( char *cmd ) {
  // process cmd line into args separated by \0
  // set pointer to each arg
  //int arg = 0;
  for ( int i = 0; cmd[i]; i++ ) {
    cmd[i] = tolower( (unsigned char)cmd[i] );
  }
  bool ws = true;
  int j = 0;
  cmdline.plen = 0;
  //int args[10];
  //char cmdargs[100];
  //for ( int i = 0; cmd[i] != '\0'; i++ ) {
  int i = 0;
  do {
    char ch = cmd[i];
    if ( ws ) {
      if ( ch != ' ' && ch != '\0' ) {
        cmdline.p[cmdline.plen] = j;
        cmdline.args[j++] = ch;
        //arg++;
        ws = false;
      }
    } else {
      if ( ch == ' ' || ch == '\0' ) {
        cmdline.args[j++] = '\0'; // will be \0 after testing done
        cmdline.plen++;
        ws = true;
      } else {
        cmdline.args[j++] = ch;
      }
    }
  } while ( cmd[i++] != 0 );
  cmdline.args[j] = '\0';
  for ( int i = cmdline.plen; i < 10; i++ ) {
    cmdline.p[i] = j;
  }
  // convenience defs
  cmdline.p0 = cmdline.args + cmdline.p[0];
  cmdline.p1 = cmdline.args + cmdline.p[1];
  cmdline.p2 = cmdline.args + cmdline.p[2];
  cmdline.p3 = cmdline.args + cmdline.p[3];
  cmdline.p4 = cmdline.args + cmdline.p[4];
  cmdline.p5 = cmdline.args + cmdline.p[5];
  exec( cmdline.p0 );
  //
  //Serial.println( cmdline.p0 );
  for ( int i = 0; i < 10; i++ ) {
    //Serial.println( cmdline.args + cmdline.p[i] );
  }
  //
  //Serial.println( cmdline.plen );
  //Serial.println( cmdline.args );
}

void cmdLine( String cmd ) {
  int bufferSize = cmd.length() + 1;
  char buffer[bufferSize];
  cmd.toCharArray( buffer, bufferSize );
  if ( strcmp( buffer, "" ) == 0 ) {
    strcpy( buffer, defcmd );
  }
  if ( rulerOn ) displayRuler();
  do_cmd( buffer );
  if ( rulerOn ) displayRuler();
}

void setupCmd() {
  println();
  println( "test cmd.main begs ..." );
  
  println( "test cmd.main ends ..." );
}
