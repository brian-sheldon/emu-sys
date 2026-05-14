# As basic as it gets nodeJS based terminal

I created this as I needed a quick and dirty terminal that I could use to connect to my various microcontrollers.  The main feature I required was the ability for the terminal to auto connect anytime the connection was lost, which is fairly often when testing code on a microcontroller.  Note:  In order to upload sketches from the Arduino IDE, this terminal must be temporarily stopped by pressing ctrl-D.  It does of course require nodeJS and some libraries installed.  Don't expect much from this as I only spent about 10 minutes making this, mostly just copying code from google search results.

The com port is currently hard coded in term.js, so it may need to be changed.

