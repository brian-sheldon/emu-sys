
#include <stdbool.h>

bool debug_disk = false;

typedef struct {
  char *src;
  char *dst;
} keymap_entry_t;

keymap_entry_t keymap[] = {
  { "1b5b337e", "" }, // del
  { "7f", "08" }, // backspace -> ctrl-h
  { "1b5b41", "05" }, // up -> ctrl-e
  { "1b5b42", "18" }, // down -> ctrl-x
  { "1b5b43", "04" }, // right -> ctrl-d
  { "1b5b44", "13" }, // left -> ctrl-s
  { "1b5b357e", "" }, // pageup
  { "1b5b367e", "" }, // pagedn
  { "1b5b46", "" }, // end
  { "1b5b347e", "" }, // end
  { "1b5b48", "" }, // home
  { "1b5b317e", "" }, // home
  { "1b5b337e", "" }, // del
  { "1b", "" }, // esc, ctrl-[
  { "1c", "" }, // ctrl-backslash
  { "1d", "" }, // ctrl-]
  { "1e", "" }, // ctrl-^
  { "1f", "" }, // ctrl-_
  { "1b4f50", "" }, // f1
  { "1b4f51", "" }, // f2
  { "1b4f52", "" }, // f3
  { "1b4f53", "" }, // f4
  { "1b5b31357e", "" }, // f5
  { "1b5b31377e", "" }, // f6
  { "1b5b31387e", "" }, // f7
  { "1b5b31397e", "" }, // f8
  { "1b5b32307e", "" }, // f9
  { "1b5b32317e", "" }, // f10
  
  { "1b5b32347e", "" }, // f12
  { NULL, NULL }
};

struct Colors {
  char *reset;
  char *prompt;

  char *ls_dir;
  char *ls_file;

  char *dump_addr;
  char *dump_hex;
  char *dump_asc;
  char *dump_label;
  char *dump_value;

  char *reg_label;
  char *reg_value;

  char *dis_label;
  char *dis_addr;
  char *dis_bytes;
  char *dis_ins;
  char *dis_comment;
  
  char *trace_label;
  char *trace_value;

  char *ruler_color;
  char *cpm_color;

  char *help_bar;
  char *help_title;
  char *help_cmd;
  char *help_params;
  char *help_desc;
};

bool rulerOn = false;
int rulerColumns = 60;

int color = 1;
struct Colors colors[2] = {
  {
    "","","","",
    "","","","",
    "","","","",
    "","","","",
    "","","","",
    "","","","",""
  },
  {
    "\x1b[0;32m","\x1b[0;31m", // reset, prompt
    "\x1b[0;36m","\x1b[0;33m", // ls - dir, file
    "\x1b[0;34m","\x1b[0;36m","\x1b[0;37m","\x1b[0;31m","\x1b[0;33m", // dump - addr, hex, ascii, label, value
    "\x1b[0;31m","\x1b[0;33m", // reg - label, value
    "\x1b[0;31m","\x1b[0;34m","\x1b[0;36m","\x1b[0;33m","\x1b[0;35m", // dis - label, addr, bytes, ins, comment
    "\x1b[0;33m","\x1b[0;37m", // trace label, value
    "\x1b[0;37m","\x1b[0;36m", // ruler - color, cpm - color
    "\x1b[0;35m","\x1b[0;36m","\x1b[0;33m","\x1b[0;31m","\x1b[0;37m" // help - bar, title, cmd, params, desc
  }
};


