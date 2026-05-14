
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

int delayTime = 0;
bool fast = true;
bool showState = false;
bool showHex = false;

struct CpuState {
  bool running;
  bool on;
  bool iowait;
  bool stopped;
  bool halted;
  bool stopset;
  uint16_t stopat;
  unsigned long ticks;
  unsigned long steps;
  bool traceCpu;
};

CpuState cpuState = { false, false, false, false, false, false, 0x0000, 0, 0, true };

bool running = false;
bool iowait = false;

int drv = 0;

int dumpaddr = 0;
