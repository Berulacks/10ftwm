10ftwm
======

A lightweight window manager designed to be used with media centers and other "10 foot" devices.

10ftwm is a fairly new project, but it aims to simplify the management of fullscreen applications in environments where a keyboard might not be available (e.g. a home-theater PC). In its current (very rough) state, the program allows you to change windows using a gamepad (or whatever is in /dev/input/js0) or keyboard.

It also comes with a little on-screen-display that is toggled by pressing the home button on your gamepad. 

### Requirements:

XCB is the only required library.


### Build instructions:

    gcc 10ftwm.c -Wall -std=c99 -ggdb -lxcb -o 10ftwm && ./10ftwm
    
...or just run the included makefile.

======


### Usage instructions:

Again, 10ftwm is still *very* early in development, but one can easily try it out with a program like [Xephyr](http://www.freedesktop.org/wiki/Software/Xephyr/), or in a new x instance.

##### Usage: 10ftwm -d [DISPLAY NAME] -s [SCREEN NUMBER]

[DISPLAY NAME]:
The display to open. Default is the contents of your $DISPLAY variable.

[SCREEN NUMBER]:
The screen number to open. Default is 0.

======


**To launch with Xephyr just open a terminal and type:**

     xephyr -screen 1024x768 -br :1

Then run the included makefile.

**To launch through a plain old X server, first compile the program, then run:**

    xinit ./10ftwm -s:1 -- :1
