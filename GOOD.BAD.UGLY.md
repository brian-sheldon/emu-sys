# The Good, the Bad and the Ugly

My thoughts on what the current state of this application.  More to come, just what I came up with off the top of my head.

## The Good

I am particularly happen with the structure I came up with for the monitor commands.  It is extremely easy to add new commmands and particularly easy to keep the commands organized into separate source files for each command category.  The cmd_entry_t structure forces the commands to have fields for the name ( required ), the parameters and short description.  So at a minimum, the help command will list ever defined commmand, even if the additional info fields are left blank.  Much better than my previous method where the help used an entirely separate structure, so I would often not update it with new commands.  This structure is not only useful for this application, but I would use it again for all future command shells, unless I come up with a better method.

I also like the number parser, function pnum(), I built, I believe it covers most of the scenerios I can think may be needed.  Mind you, there is yet no parsing for floating point numbers, which up to now are not really required by this application.

The input IO layering works reasonably well, but there may be areas of improvement, mainly in the quality of the code.

## The Bad

The global variables for config, state and other, really need to be better organized, named and put into better structures.

The arrays and structures built to organize the disk images and the connections to the monitor and system, need some work ( especially after I started transitioning to a new structure ).  I also need to add in way to change image files.

One additional point related to the structures for disk images and connections.  There is definitely an issue with the structures as they seemed to be related to issues that only appeared when I ran the linux version of this application in Termux.  Before working on this change however, I want to further investigate this issue as I am hoping to understand exactly why this was happening.  Otherwise, I may have a potential bug in the code that is not currently causing issues, but may shown up at some point.

## The Ugly

I wanted to have two generic functions for sending any kind of data to the output.  Unfortunately, I am continually having to extend these functions for new data types.  Not to mention that the ones already built are a bit hacky due to issues with both function overloading in Arduino and generic functions in standard C.  For example, int and char are seen as the same type, which is why I have created separate functions for sending individual characters.  I am still trying to decide how best to resolve this, maybe just by using specific functions for every type which would require a lot of code changes.

The config in particular, should eventually be placed into a config file to avoid having to recompiled for every config change.

This application really needs more error checking and user input validation, enough said.

I should try and get around to making at least some basic get started docs, not just what was mentioned in that ever increasingly long README.md file.



