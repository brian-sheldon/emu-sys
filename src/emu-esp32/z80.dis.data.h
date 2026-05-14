
//
// Extracted from the emulator found at
//
//   https://trsjs.48k.ca/
//
// and written by 
//
//   Peter Phillips
//
// modified by Brian Sheldon
//

#include <string.h>

const char *major[] = {
  "nop",
  "ld bc,%W",
  "ld (bc),a",
  "inc bc",
  "inc b",
  "dec b",
  "ld b,%B",
  "rlca",

  "ex af,af'",
  "add hl,bc",
  "ld a,(bc)",
  "dec bc",
  "inc c",
  "dec c",
  "ld c,%B",
  "rrca",

  "djnz %J",
  "ld de,%W",
  "ld (de),a",
  "inc de",
  "inc d",
  "dec d",
  "ld d,%B",
  "rla",

  "jr %J",
  "add hl,de",
  "ld a,(de)",
  "dec de",
  "inc e",
  "dec e",
  "ld e,%B",
  "rra",

  "jr nz,%J",
  "ld hl,%W",
  "ld (%A),hl",
  "inc hl",
  "inc h",
  "dec h",
  "ld h,%B",
  "daa",

  "jr z,%J",
  "add hl,hl",
  "ld hl,(%A)",
  "dec hl",
  "inc l",
  "dec l",
  "ld l,%B",
  "cpl",

  "jr nc,%J",
  "ld sp,%W",
  "ld (%A),a",
  "inc sp",
  "inc (hl)",
  "dec (hl)",
  "ld (hl),%B",
  "scf",

  "jr c,%J",
  "add hl,sp",
  "ld a,(%A)",
  "dec sp",
  "inc a",
  "dec a",
  "ld a,%B",
  "ccf",

  "ld b,b",
  "ld b,c",
  "ld b,d",
  "ld b,e",
  "ld b,h",
  "ld b,l",
  "ld b,(hl)",
  "ld b,a",

  "ld c,b",
  "ld c,c",
  "ld c,d",
  "ld c,e",
  "ld c,h",
  "ld c,l",
  "ld c,(hl)",
  "ld c,a",

  "ld d,b",
  "ld d,c",
  "ld d,d",
  "ld d,e",
  "ld d,h",
  "ld d,l",
  "ld d,(hl)",
  "ld d,a",

  "ld e,b",
  "ld e,c",
  "ld e,d",
  "ld e,e",
  "ld e,h",
  "ld e,l",
  "ld e,(hl)",
  "ld e,a",

  "ld h,b",
  "ld h,c",
  "ld h,d",
  "ld h,e",
  "ld h,h",
  "ld h,l",
  "ld h,(hl)",
  "ld h,a",

  "ld l,b",
  "ld l,c",
  "ld l,d",
  "ld l,e",
  "ld l,h",
  "ld l,l",
  "ld l,(hl)",
  "ld l,a",

  "ld (hl),b",
  "ld (hl),c",
  "ld (hl),d",
  "ld (hl),e",
  "ld (hl),h",
  "ld (hl),l",
  "halt",
  "ld (hl),a",

  "ld a,b",
  "ld a,c",
  "ld a,d",
  "ld a,e",
  "ld a,h",
  "ld a,l",
  "ld a,(hl)",
  "ld a,a",

  "add a,b",
  "add a,c",
  "add a,d",
  "add a,e",
  "add a,h",
  "add a,l",
  "add a,(hl)",
  "add a,a",

  "adc a,b",
  "adc a,c",
  "adc a,d",
  "adc a,e",
  "adc a,h",
  "adc a,l",
  "adc a,(hl)",
  "adc a,a",

  "sub b",
  "sub c",
  "sub d",
  "sub e",
  "sub h",
  "sub l",
  "sub (hl)",
  "sub a",

  "sbc a,b",
  "sbc a,c",
  "sbc a,d",
  "sbc a,e",
  "sbc a,h",
  "sbc a,l",
  "sbc a,(hl)",
  "sbc a,a",

  "and b",
  "and c",
  "and d",
  "and e",
  "and h",
  "and l",
  "and (hl)",
  "and a",

  "xor b",
  "xor c",
  "xor d",
  "xor e",
  "xor h",
  "xor l",
  "xor (hl)",
  "xor a",

  "or b",
  "or c",
  "or d",
  "or e",
  "or h",
  "or l",
  "or (hl)",
  "or a",

  "cp b",
  "cp c",
  "cp d",
  "cp e",
  "cp h",
  "cp l",
  "cp (hl)",
  "cp a",

  "ret nz",
  "pop bc",
  "jp nz,%A",
  "jp %A",
  "call nz,%C",
  "push bc",
  "add a,%B",
  "rst %R",

  "ret z",
  "ret",
  "jp z,%A",
  "0",
  "call z,%C",
  "call %C",
  "adc a,%B",
  "rst %R",

  "ret nc",
  "pop de",
  "jp nc,%A",
  "out (%P),a",
  "call nc,%C",
  "push de",
  "sub %B",
  "rst %R",

  "ret c",
  "exx",
  "jp c,%A",
  "in a,(%P)",
  "call c,%C",
  "1",
  "sbc a,%B",
  "rst %R",

  "ret po",
  "pop hl",
  "jp po,%A",
  "ex (sp),hl",
  "call po,%C",
  "push hl",
  "and %B",
  "rst %R",

  "ret pe",
  "jp (hl)",
  "jp pe,%A",
  "ex de,hl",
  "call pe,%C",
  "2",
  "xor %B",
  "rst %R",

  "ret p",
  "pop af",
  "jp p,%A",
  "di",
  "call p,%C",
  "push af",
  "or %B",
  "rst %R",

  "ret m",
  "ld sp,hl",
  "jp m,%A",
  "ei",
  "call m,%C",
  "1",
  "cp %B",
  "rst %R"
};


const char *minor[][256] = {
  {	//cb
    "rlc b",
    "rlc c",
    "rlc d",
    "rlc e",
    "rlc h",
    "rlc l",
    "rlc (hl)",
    "rlc a",

    "rrc b",
    "rrc c",
    "rrc d",
    "rrc e",
    "rrc h",
    "rrc l",
    "rrc (hl)",
    "rrc a",

    "rl b",
    "rl c",
    "rl d",
    "rl e",
    "rl h",
    "rl l",
    "rl (hl)",
    "rl a",

    "rr b",
    "rr c",
    "rr d",
    "rr e",
    "rr h",
    "rr l",
    "rr (hl)",
    "rr a",

    "sla b",
    "sla c",
    "sla d",
    "sla e",
    "sla h",
    "sla l",
    "sla (hl)",
    "sla a",

    "sra b",
    "sra c",
    "sra d",
    "sra e",
    "sra h",
    "sra l",
    "sra (hl)",
    "sra a",

    "sll b", // undefined
    "sll c", // undefined
    "sll d", // undefined
    "sll e", // undefined
    "sll h", // undefined
    "sll l", // undefined
    "sll (hl)", // undefined
    "sll a", // undefined

    "srl b",
    "srl c",
    "srl d",
    "srl e",
    "srl h",
    "srl l",
    "srl (hl)",
    "srl a",

    "bit 0,b",
    "bit 0,c",
    "bit 0,d",
    "bit 0,e",
    "bit 0,h",
    "bit 0,l",
    "bit 0,(hl)",
    "bit 0,a",

    "bit 1,b",
    "bit 1,c",
    "bit 1,d",
    "bit 1,e",
    "bit 1,h",
    "bit 1,l",
    "bit 1,(hl)",
    "bit 1,a",

    "bit 2,b",
    "bit 2,c",
    "bit 2,d",
    "bit 2,e",
    "bit 2,h",
    "bit 2,l",
    "bit 2,(hl)",
    "bit 2,a",

    "bit 3,b",
    "bit 3,c",
    "bit 3,d",
    "bit 3,e",
    "bit 3,h",
    "bit 3,l",
    "bit 3,(hl)",
    "bit 3,a",

    "bit 4,b",
    "bit 4,c",
    "bit 4,d",
    "bit 4,e",
    "bit 4,h",
    "bit 4,l",
    "bit 4,(hl)",
    "bit 4,a",

    "bit 5,b",
    "bit 5,c",
    "bit 5,d",
    "bit 5,e",
    "bit 5,h",
    "bit 5,l",
    "bit 5,(hl)",
    "bit 5,a",

    "bit 6,b",
    "bit 6,c",
    "bit 6,d",
    "bit 6,e",
    "bit 6,h",
    "bit 6,l",
    "bit 6,(hl)",
    "bit 6,a",

    "bit 7,b",
    "bit 7,c",
    "bit 7,d",
    "bit 7,e",
    "bit 7,h",
    "bit 7,l",
    "bit 7,(hl)",
    "bit 7,a",

    "res 0,b",
    "res 0,c",
    "res 0,d",
    "res 0,e",
    "res 0,h",
    "res 0,l",
    "res 0,(hl)",
    "res 0,a",

    "res 1,b",
    "res 1,c",
    "res 1,d",
    "res 1,e",
    "res 1,h",
    "res 1,l",
    "res 1,(hl)",
    "res 1,a",

    "res 2,b",
    "res 2,c",
    "res 2,d",
    "res 2,e",
    "res 2,h",
    "res 2,l",
    "res 2,(hl)",
    "res 2,a",

    "res 3,b",
    "res 3,c",
    "res 3,d",
    "res 3,e",
    "res 3,h",
    "res 3,l",
    "res 3,(hl)",
    "res 3,a",

    "res 4,b",
    "res 4,c",
    "res 4,d",
    "res 4,e",
    "res 4,h",
    "res 4,l",
    "res 4,(hl)",
    "res 4,a",

    "res 5,b",
    "res 5,c",
    "res 5,d",
    "res 5,e",
    "res 5,h",
    "res 5,l",
    "res 5,(hl)",
    "res 5,a",

    "res 6,b",
    "res 6,c",
    "res 6,d",
    "res 6,e",
    "res 6,h",
    "res 6,l",
    "res 6,(hl)",
    "res 6,a",

    "res 7,b",
    "res 7,c",
    "res 7,d",
    "res 7,e",
    "res 7,h",
    "res 7,l",
    "res 7,(hl)",
    "res 7,a",

    "set 0,b",
    "set 0,c",
    "set 0,d",
    "set 0,e",
    "set 0,h",
    "set 0,l",
    "set 0,(hl)",
    "set 0,a",

    "set 1,b",
    "set 1,c",
    "set 1,d",
    "set 1,e",
    "set 1,h",
    "set 1,l",
    "set 1,(hl)",
    "set 1,a",

    "set 2,b",
    "set 2,c",
    "set 2,d",
    "set 2,e",
    "set 2,h",
    "set 2,l",
    "set 2,(hl)",
    "set 2,a",

    "set 3,b",
    "set 3,c",
    "set 3,d",
    "set 3,e",
    "set 3,h",
    "set 3,l",
    "set 3,(hl)",
    "set 3,a",

    "set 4,b",
    "set 4,c",
    "set 4,d",
    "set 4,e",
    "set 4,h",
    "set 4,l",
    "set 4,(hl)",
    "set 4,a",

    "set 5,b",
    "set 5,c",
    "set 5,d",
    "set 5,e",
    "set 5,h",
    "set 5,l",
    "set 5,(hl)",
    "set 5,a",

    "set 6,b",
    "set 6,c",
    "set 6,d",
    "set 6,e",
    "set 6,h",
    "set 6,l",
    "set 6,(hl)",
    "set 6,a",

    "set 7,b",
    "set 7,c",
    "set 7,d",
    "set 7,e",
    "set 7,h",
    "set 7,l",
    "set 7,(hl)",
    "set 7,a"
  },
  {	// dd fd
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "add ix,bc",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "add ix,de",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "ld ix,%W",
    "ld (%A),ix",
    "inc ix",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "add ix,ix",
    "ld ix,(%A)",
    "dec ix",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "inc (ix%I)",
    "dec (ix%I)",
    "ld (ix%I),%L",
    "undefined",

    "undefined",
    "add ix,sp",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld b,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld c,(ix%I)",
    "undefined",
      
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld d,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld e,(ix%I)",
    "undefined",
      
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld h,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld l,(ix%I)",
    "undefined",
      
    "ld (ix%I),b",
    "ld (ix%I),c",
    "ld (ix%I),d",
    "ld (ix%I),e",
    "ld (ix%I),h",
    "ld (ix%I),l",
    "undefined",
    "ld (ix%I),a",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "ld a,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "add a,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "adc a,(ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "sub (ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "sbc a,(ix%I)",
    "undefined",
      
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "and (ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "xor (ix%I)",
    "undefined",
      
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "or (ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "cp (ix%I)",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "3",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "pop ix",
    "undefined",
    "ex (sp),ix",
    "undefined",
    "push ix",
    "undefined",
    "undefined",

    "undefined",
    "jp (ix)",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "ld sp,ix",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined"
  },
  { // ed
    // 00
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 08
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 10
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 18
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 20
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 28
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 30
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 38
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    // 40
    "in b,(c)",
    "out (c),b",
    "sbc hl,bc",
    "ld (%A),bc",
    "neg",
    "retn",
    "im 0",
    "ld i,a",
    // 48
    "in c,(c)",
    "out (c),c",
    "adc hl,bc",
    "ld bc,(%A)",
    "undefined",
    "reti",
    "undefined",
    "ld r,a",
    // 50
    "in d,(c)",
    "out (c),d",
    "sbc hl,de",
    "ld (%A),de",
    "undefined",
    "undefined",
    "im 1",
    "ld a,i",
    // 58
    "in e,(c)",
    "out (c),e",
    "adc hl,de",
    "ld de,(%A)",
    "undefined",
    "undefined",
    "im 2",
    "ld a,r",
    // 60
    "in h,(c)",
    "out (c),h",
    "sbc hl,hl",
    "ld (%A),hl    ; undocumented",
    "undefined",
    "undefined",
    "undefined",
    "rrd",

    "in l,(c)",
    "out (c),l",
    "adc hl,hl",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "rld",

    "undefined",
    "undefined",
    "sbc hl,sp",
    "ld (%A),sp",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "in a,(c)",
    "out (c),a",
    "adc hl,sp",
    "ld sp,(%A)",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "ldi",
    "cpi",
    "ini",
    "outi",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "ldd",
    "cpd",
    "ind",
    "outd",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "ldir",
    "cpir",
    "inir",
    "otir",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "lddr",
    "cpdr",
    "indr",
    "otdr",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",

    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined",
    "undefined"
  }
};

/*

function hex2(v)
{
	var fmt = "00" + v.toString(16);
	return fmt.substring(fmt.length - 2);
}

function hex4(v)
{
	var fmt = "0000" + v.toString(16);
	return fmt.substring(fmt.length - 4);
}

function fetch(s)
{
	var res = s.mem[(s.PC + s._len) & 0xffff];
	s._len++;

	return res;
}

function fetchword(s)
{
	return fetch(s) | (fetch(s) << 8);
}

function displacement(x)
{
	x &= 0xff;
	if (x >= 128) x -= 256;
	return x;
}

function format_displacement(fmt, op)
{
	if (op >= 0) {
		fmt = fmt.replace('%I', '+' + hex2(op));
	} else {
		fmt = fmt.replace('%I', '-' + hex2((op & 255 ^ 255) + 1));
	}

	return fmt;
}

// Returns list of two lists
// First is the disassembly.
// Second is the integer opcodes.
// The disassembly list separates operation from arguments.
function dis1( mem, addr ) {
  let state = {};
  state.mem = mem;
  state.PC = addr;
  let res = z80dis( state );
  let parts = res[ 0 ];
  inst  = parts[0].padEnd( 6, ' ' );
  inst += parts.length > 1 ? parts[1] : '' ;
  inst += parts.length > 2 ? ',' + parts[2] : '' ;
  inst += parts.length > 3 ? ',' + parts[3] : '' ;
  let bytes = res[ 1 ];
  let nextaddr = addr + bytes.length;
  return [ nextaddr, bytes, inst ];
}

function z80dis(s) {
  var i, res, op, op0, fmt, minormode, fmt3dis = 0;
  
  s._len = 0;
  op = fetch(s);
  //log(op);
  op0 = op;
  res = [];
  fmt = major[op];
  //log(fmt);
  minormode = -1;
  
  while (fmt == 0 || fmt == 1 || fmt == 2 || fmt == 3) {
    minormode = fmt;
    if (fmt == 3) {
      fmt = 0;
      fmt3dis = displacement(fetch(s));
    }
    op = fetch(s);
    fmt = minor[fmt][op];
  }
  
  if (minormode == 3) {	// dd cb or fd cb
    if (fmt.indexOf('(hl)') < 0) {
      fmt = "undefined";
    } else {
      fmt = fmt.replace('(hl)','(ix%I)');
    }
    fmt = format_displacement(fmt, fmt3dis);
  } else if (fmt.indexOf('%B') >= 0) {	// BC
    fmt = fmt.replace('%B', hex2(fetch(s)));
  } else if (fmt.indexOf('%W') >= 0) {	// WC
    fmt = fmt.replace('%W', hex4(fetchword(s)));
  } else if (fmt.indexOf('%A') >= 0) {	// ADDR
    fmt = fmt.replace('%A', hex4(fetchword(s)));
  } else if (fmt.indexOf('%J') >= 0) {	// JR
    op = fetch(s);
    op = (s.PC + s._len + displacement(op)) & 0xffff;
    fmt = fmt.replace('%J', hex4(op));
  } else if (fmt.indexOf('%I') >= 0) {	// IND
    op = displacement(fetch(s));
    fmt = format_displacement(fmt, op);
    if (fmt.indexOf('%L') >= 0) {	// IND_1
      fmt = fmt.replace('%L', hex2(fetch(s)));
    }
  } else if (fmt.indexOf('%C') >= 0) {	// CALL
    fmt = fmt.replace('%C', hex4(fetchword(s)));
  } else if (fmt.indexOf('%R') >= 0) {	// RST
    fmt = fmt.replace('%R', hex2(op0 & 0x38));
  } else if (fmt.indexOf('%P') >= 0) {	// PORT
    fmt = fmt.replace('%P', hex2(fetch(s)));
  }
  
  if (op0 == 0xfd) {
    fmt = fmt.replace('ix', 'iy');
  }
  
  res[0] = fmt.split(' ');
  res[1] = [];
  for (i = 0; i < s._len; i++) {
    var byt = s.mem[ ( s.PC + i ) & 0xffff ];
    res[1][i] = byt;
  }
  
  return res;
}

	var api = {
		'dis': function(state) { return z80dis(state); },
    'dis1' : function( mem, addr ) { return dis1( mem, addr ); }
	};

	return api;
}());

if ( typeof window !== 'object' ) {
  module.exports = Z80Dis;
}

*/




