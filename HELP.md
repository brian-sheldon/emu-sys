# Help List from App

```
>>> help
=====  cpu  ===================================================================
mhz                          ; cpu mhz
d         [addr]             ; mem dump at next addr or given addr
l         [addr]             ; disassembly listing for addr
step      [steps]            ; cpu step once or given steps
state                        ; cpu state
next                         ; show next instruction to run
stopclr                      ; stopat clr
stopset                      ; stopat set
stopat    [addr]             ; stopat addr
pc        [addr]             ; show or set reg pc
on                           ; turn cpu on
1b5b317e                     ; end key - turn cpu on
off                          ; turn cpu off
1b5b347e                     ; home key - turn cpu off
=====  trace  =================================================================
cclr                         ; clear cpu trace data
cset      [addr]             ; set cpu trace addr start
cshow     [beg] [end] [min]  ; show num of op fetches at addr over min
mclr                         ; clear mem trace data
mset      [addr]             ; set mem trace addr start
mshow     [beg] [end] [min]  ; show num of mem rd wr at addr over min
=====  disk  ==================================================================
b                            ; load boot sector into zero page
drv       [index]            ; current drv index or set drv index
drvs                         ; list configured drvs
img       [index] [path]     ; list img or set img path
imgs                         ; list configured imgs
disk      [trk logsec]       ; hexdump of next sector or sector at trk logsec
trklog    trk logsec         ; get blk blksec from trk logsec
blksec    blk blksec         ; get trk logsec from blk blksec
=====  fs  ====================================================================
ls        path               ; disk dir
rm        path               ; rm file
cp        src dst            ; cp file
cmp       file1 file2        ; cmp files
mkdir     path               ; mkdir
rmdir     path               ; rmdir
=====  main  ==================================================================
clrcmd                       ; clears the current cmd repeated when cmdline is
ruler     [columns]          ; display ruler and set length
ruleron                      ; ruler on for every cmd
ruleroff                     ; ruler off
colors                       ; display colors
help                         ; detailed help for all commands
words                        ; list all commands
>>>
```

