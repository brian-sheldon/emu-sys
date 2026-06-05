
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

#include <stdint.h>
#include <stdbool.h>

char defcmd[20] = "";

bool m_help_displayed = false;
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
  uint64_t ticks;
  uint64_t steps;
  uint64_t frames;
  float mhz;
  float mhzMin;
  float mhzMax;
  bool traceCpu;
};

struct CpuState cpuState = { false, false, false, false, false, false, 0x0000, 0, 0, 0, 0.0, 2000.0, 0.0, true };

bool running = false;
bool iowait = false;

int drv = 0;

int dumpaddr = 0;

int port66Status = 0;

//
// cp/m disk img
//

#ifdef ESP32

char *cpm_imgs[] = {
  "/emu/disks/cpm22-1.dsk",
  "/emu/disks/cpm22-2.dsk",
  "/emu/disks/8080tools.cpm",
  "/emu/disks/trek.cpm",
  "", "", "", "",
  "/emu/disks/hd1.dsk",
  "/emu/disks/hd2.dsk",
  "", "", "", "",
  "", "", "", ""
};

#else

char *cpm_imgs[] = {
  "../../sdcard/emu/disks/cpm22-1.dsk",
  "../../sdcard/emu/disks/cpm22-2.dsk",
  "../../sdcard/emu/disks/8080tools.cpm",
  "../../sdcard/emu/disks/trek.cpm",
  "", "", "", "",
  "../../sdcard/emu/disks/hd1.dsk",
  "../../sdcard/emu/disks/hd2.dsk",
  "", "",
  "", "", "", ""
};

#endif

int cpm_drvs[] = {
  0, 1, 2, 3,
  4, 5, 6, 7,
  8, 9, 10, 11,
  12, 13, 14, 15
};

struct Mon_drv {
  int img;
  int trk;
  int log;
};

struct Mon_drv mon_drvs[] = {
  { 0, 0, 1 },
  { 1, 0, 1 },
  { 2, 0, 1 },
  { 3, 0, 1 },
  { 4, 0, 1 },
  { 5, 0, 1 },
  { 6, 0, 1 },
  { 7, 0, 1 },
  { 8, 0, 1 },
  { 9, 0, 1 },
  { 10, 0, 1 },
  { 11, 0, 1 },
  { 12, 0, 1 },
  { 13, 0, 1 },
  { 14, 0, 1 },
  { 15, 0, 1 }
};


