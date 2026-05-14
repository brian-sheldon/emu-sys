
// Copyright (C) 2026 Brian Sheldon
//
// MIT License

void fs_ls() {
  EmuFileUtil fsutil;
  if ( strcmp( cmdline.p1, "" ) != 0 ) {
    fsutil.ls( cmdline.p1 );
  } else {
    fsutil.ls( "/emu/disks" );
  }
}

void fs_rm() {
  EmuFileUtil fsutil;
  if ( strcmp( cmdline.p1, "" ) != 0 ) {
    fsutil.rm( cmdline.p1 );
  }
}

void fs_cp() {
  EmuFileUtil fsutil;
  if ( strcmp( cmdline.p1, "" ) != 0 ) {
    if ( strcmp( cmdline.p2, "" ) != 0 ) {
      fsutil.cp( cmdline.p1, cmdline.p2 );
    }
  }
}

void fs_cmp() {
  EmuFileUtil fsutil;
  if ( strcmp( cmdline.p1, "" ) != 0 ) {
    if ( strcmp( cmdline.p2, "" ) != 0 ) {
      fsutil.cmp( cmdline.p1, cmdline.p2 );
    }
  }
}

void fs_mkdir() {
  //
}

void fs_rmdir() {
  //
}

cmd_entry_t cmds_fs[] = {
  { "ls", fs_ls, "path", "disk dir" },
  { "rm", fs_rm, "path", "rm file" },
  { "cp", fs_cp, "src dst", "cp file" },
  { "cmp", fs_cmp, "file1 file2", "cmp files" },
  { "mkdir", fs_mkdir, "path", "mkdir" },
  { "rmdir", fs_rmdir, "path", "rmdir" },
  { NULL, NULL, NULL, NULL }
};
