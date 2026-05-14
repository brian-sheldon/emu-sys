
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

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

Cmdline cmdline;
