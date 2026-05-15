
#include <stdbool.h>

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


