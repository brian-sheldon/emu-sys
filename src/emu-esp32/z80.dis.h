
//
// Extracted from the emulator found at
//
//   https://trsjs.48k.ca/
//
// and written by 
//
//   Peter Phillips
//
// Modified and Converted to C by Brian Sheldon
//

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "z80.dis.data.h"

struct Disassem {
  int addr;
  int op;
  int byt;
  uint8_t opc[4];
  uint8_t bytes[6];
  char ins[8];
};

void strReplace( char *str, size_t str_size, const char *old_sub, const char *new_sub ) {
  char *pos = strstr(str, old_sub);
  if (!pos) return; // Substring not found
  size_t old_len = strlen(old_sub);
  size_t new_len = strlen(new_sub);
  size_t tail_len = strlen(pos + old_len) + 1; // +1 to include null terminator
  // show check if can fit
  memmove(pos + new_len, pos + old_len, tail_len);
  memcpy(pos, new_sub, new_len);
}

/*
void insStr( char *dst, int index, char *src ) {
  int len = strlen( src );
  //memmove
  //dst[index] = ch;
}

char *dec0( int v, int width = 0 ) {
  static char buffer[ 40 ];
  snprintf( buffer, sizeof( buffer ), "%*d", width, v );
  return buffer;
}

char *hex0( int v, int width = 0 ) {
  static char buffer[ 20 ];
  snprintf( buffer, sizeof( buffer ), "%0*x", width, v );
  return buffer;
}

char *hex2( int v ) {
  return hex0( v, 2 );
}

char *hex4( int v ) {
  return hex0( v, 4 );
}
*/

int dis( uint8_t data[], int addr ) {
  const char *ixy[] = { "ix", "iy" };
  Disassem res;
  res.addr = addr;
  res.op = 0;
  res.byt = 0;
  uint8_t disp;
  char ins[50];
  int op0 = data[ addr++ ];
  res.opc[ res.op ] = op0;
  res.bytes[ res.byt++ ] = res.opc[ res.op ];
  strcpy( ins, major[ res.opc[ res.op++ ] ] );
  int ind;
  while ( strlen( ins ) == 1 ) {
    ind = atoi( ins );
    if ( ind == 3 ) {
      disp = data[ addr++ ];
      res.bytes[ res.byt++ ] = disp;
    }
    res.opc[ res.op ] = data[ addr++ ];
    res.bytes[ res.byt++ ]  = res.opc[ res.op ];
    strcpy( ins, minor[ ind ][ res.opc[ res.op++ ] ] );
  }
  int high, low, sb, absaddr;
  char *i;
  if ( ind == 3 ) { // dd cb or fd cb
    if ( strstr( ins, "(HL)" ) ) {
      strcpy( ins, "undefined" );
    } else {
      strReplace( ins, sizeof( ins ), "(HL)", "(IX%I)" );
    }
  } else if ( strstr( ins, "%B" ) ) { // b
    low = data[addr++];
    res.bytes[ res.byt++ ] = low;
    strReplace( ins, sizeof( ins ), "%B", "$%B" );
    strReplace( ins, sizeof( ins ), "%B", hex2( low ) );
  } else if ( strstr( ins, "%W" ) ) { // w
    low = data[addr++];
    high = data[addr++];
    res.bytes[ res.byt++ ] = low;
    res.bytes[ res.byt++ ] = high;
    strReplace( ins, sizeof( ins ), "%W", "$%W" );
    strReplace( ins, sizeof( ins ), "%W", hex4( high<<8 | low ) );
  } else if ( strstr( ins, "%A" ) ) { // addr
    low = data[addr++];
    high = data[addr++];
    res.bytes[ res.byt++ ] = low;
    res.bytes[ res.byt++ ] = high;
    strReplace( ins, sizeof( ins ), "%A", "$%A" );
    strReplace( ins, sizeof( ins ), "%A", hex4( high<<8 | low ) );
  } else if ( strstr( ins, "%J" ) ) { // jr
    low = data[addr++];
    res.bytes[ res.byt++ ] = low;
    sb = low;
    if ( low > 127 ) {
      sb = low - 256;
    }
    absaddr = addr + sb;
    strReplace( ins, sizeof( ins ), "%J", "$%J" );
    strReplace( ins, sizeof( ins ), "%J", hex4( absaddr ) );
  } else if ( i = strstr( ins, "%I" ) ) { // ind_l
    low = data[addr++];
    res.bytes[ res.byt++ ] = low;
    if ( low < 128 ) {
      strReplace( ins, sizeof( ins ), "%I", "+%I" );
    } else {
      low = 256 - low;
      strReplace( ins, sizeof( ins ), "%I", "-%I" );
    }
    strReplace( ins, sizeof( ins ), "%I", dec0( low ) );
    if ( strstr( ins, "%L" ) ) {
      low = data[addr++];
      res.bytes[ res.byt++ ] = low;
      strReplace( ins, sizeof( ins ), "%L", "$%L" );
      strReplace( ins, sizeof( ins ), "%L", hex2( low ) );
    }
  } else if ( i = strstr( ins, "%C" ) ) { // call
    low = data[addr++];
    high = data[addr++];
    res.bytes[ res.byt++ ] = low;
    res.bytes[ res.byt++ ] = high;
    strReplace( ins, sizeof( ins ), "%C", "$%C" );
    strReplace( ins, sizeof( ins ), "%C", hex4( high<<8 | low ) );
  } else if ( i = strstr( ins, "%R" ) ) { // rst
    low = data[addr++];
    res.bytes[ res.byt++ ] = low;
    strReplace( ins, sizeof( ins ), "%R", hex2( low ) );
  } else if ( i = strstr( ins, "%P" ) ) { // port
    low = data[addr++];
    res.bytes[ res.byt++ ] = low;
    strReplace( ins, sizeof( ins ), "%P", "$%P" );
    strReplace( ins, sizeof( ins ), "%P", hex2( low ) );
  }

  if ( op0 == 0xfd ) {
    strReplace( ins, sizeof( ins ), "IX", "IY" );
  }


  //strcpy( res.ins, ins );
  char bytesStr[20];
  for ( int i = 0; i < 4; i++ ) {
    memcpy( bytesStr + i * 3, "   ", 3 );
    if ( i < res.byt ) {
      memcpy( bytesStr + i * 3, hex2( res.bytes[i] ), 2 );
    }
  }
  bytesStr[3*3+2] = '\0';
  char pbuf[100];
  snprintf( pbuf, sizeof( pbuf ), "%s%04x  %s%s  %s%s", colors[color].dis_addr, res.addr, colors[color].dis_bytes, bytesStr, colors[color].dis_ins, ins );
  println( pbuf );
  return res.byt;
}



