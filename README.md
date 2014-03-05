10ftwm
======

A lightweight window manager designed to be used with media centers and other "10 foot" devices.

10ftwm is still a new project, but here are the features I aim to include in it:
* Controller/Joystick support
* LIRC support
* Rudimentary process management (for dealing with things like crashes)
* An interface :)

**Requirements:**

XCB is the only required library.

**Build instructions:**

Please use the included makefile. It's as simple as simple gets.


**Usage instructions:**

Again, 10ftwm is still *very* early in development, but one can easily try it out with a program like [Xephyr](http://www.freedesktop.org/wiki/Software/Xephyr/), or in a new x instance. Please note that the program only runs on display :1 right now, so you'll have to name your x instance accordingly.

*To launch with Xephyr just open a terminal and type:*

     xephyr -screen 1024x768 -br :1

Then run the included makefile.

*To launch through a plain old X server, first compile the program, then run:*

    xinit ./10ftwm -- :1
