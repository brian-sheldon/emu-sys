
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#define CMD_MODE_MAIN 0
#define CMD_MODE_FORTH 1

bool cmd_mode = CMD_MODE_MAIN;

typedef void (*cmd_func_t)(void);

typedef struct {
  const char *name;
  cmd_func_t func;
  const char *params;
  const char *desc;
} cmd_entry_t;


struct Cmdline {
  char cmd[100+1];
  char args[100+1];
  int plen;
  int p[10];
  char *p0;
  char *p1;
  char *p2;
  char *p3;
  char *p4;
  char *p5;
};

struct Cmdline cmdline;
