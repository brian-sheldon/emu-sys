# A barely working example of a TRS-80 Model 1

This is by far a totally incomplete emulation of a TRS-80 Model 1.  It is only able to barely function as I modified a small portion of the model1.rom to get keyboard input from the terminal and send output to the terminal.  The problem is that not all method of input and output go through these functions.  There is no graphics characters as the screen memory is not emulated.  Nor do a few of the keyboard keys function, such as the Break key, as it is not processed in the same way as many other keys.  So the only way to stop a program is to reset the M5Cardputer.  Also, Basic does not use the Backspace key, so Ctrl-h is needed instead.

To run the Model 1 rom, it first needs to be loaded at address 0, via the command "load /emu/roms/model1.rom".  Then the rom needs to be modified using the command "patch".  At this point, the cpu can be started with the command "on", followed by the PageUp key to connect the keyboard to the emulated system.

I also have a binary for a forth built to run on the TRS-80. It needs to be loaded at address 8000.

To run this forth, first stop the cpu using "off", then you need to reconnect the keyboard to the CLI using the PageDown key.  The "load /emu/roms/forth.bin 8000" loads the forth into memory.  The cpu program counter needs to be set to address 8000 using "pc 8000".  Then the cpu can be started with "on".

Obviously, this is not very functional.  I included this to show how it is possible to adapt this emulation to another platform.  This example shows how easy it can sometimes be to just get a partially functioning system running.  This can make it easier to then build the full system, as it is only necessary to build one piece at a time, ensuring the system is still running after each new piece is added.

### Sample session

```
>>> ls /emu/roms
forth.bin  16384
model1.rom  12288
>>> load /emu/roms/model1.rom 0
Binary size: 12288 loaded at addr: 0
>>> patch
>>> on
cpu: on
>>> MEMORY SIZE?
io directed to cpu ...

RADIO SHACK LEVEL II BASIC
READY
>10 for x=1to10
>20 print x
>30 next x
>list
10 FOR X =1TO10
20 PRINT X
30 NEXT X
READY
>run
 1
 2
 3
 4
 5
 6
 7
 8
 9
 10
READY
>
io directed to cli ...

>>> off
>>> load /emu/roms/forth.bin 8000
Binary size: 16384 loaded at addr: 32768
>>> pc 8000
32768
>>> on
cpu: on
>>> TRS-80 Forth
(c) 2022 Lawrence Kesteloot

Initializing.........................................
ok

io directed to cpu ...

ok
words
demo-star demo-lines demo-points ry rx line line-v line-m line-dy line-dx line-y2 line-y1 line-x2 line-x1 diff swapvar pick 1+ tuck nip u. hex decimal recurse words space loop do k j i doloopptr repeat while again until begin <> >= > <= 2dup not else then if variable constant mod / dsp@ base here state latest height width key rndn rnd ; : immediate find 0branch branch word >cfa create code ' ] [ vline hline cls point reset set , lit cr tell inkey emit .b . < = makeword highbyte /mod * - + 8<< 8>> invert or and io! io@ ! @ -rot rot over swap drop ?dup dup exit enter
ok
111 222 + .
333 ok
```



