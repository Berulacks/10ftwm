10ftwm
======

A lightweight window manager designed to be used with media centers and other "10 foot" devices.

10ftwm is a fairly new project, but it aims to simplify the management of fullscreen applications in environments where a keyboard might not be available (e.g. a home-theater PC). In its current (very rough) state, the program allows you to change windows using a gamepad (or whatever is in /dev/input/js0) or keyboard.

It also comes with a little on-screen-display that is toggled by pressing the home button on your gamepad. 

### Requirements:

XCB and lirc are required.


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

    xinit ./10ftwm -d:1 -- :1
    
======
    
### Configuration:

10ftwm can be configured through a handy file called 10ftwmrc placed in the same directory as the executable. The file should contain simple key-value pairs:

    key = value
    key = value
    (...) (ad infitum)
    
the currently support options are:

    screen = int //The screen number to run on
    display = string //The display name to run on
    OSD_button = int //The gamepad button used to toggle the OSD
    OSD_next_ws = int //The gamepad button used to switch to the next workspace
    OSD_previous_ws = int
    js_file = string //The file to open for the joystick file descriptor. Defaults to /dev/input/js0
    lirc_config = string //The path to your lirc configuration file, defaults to ~/lircrc or /etc/lircrc
    exec = string //Execute this program on launch.
    exec = string
    (...) (ad infinitum)

======

### lirc:

10ftwm has full lirc support, simply create a ~/.lircrc file (or pass one in the config file) and map your buttons of choice to:

* TOGGLE_OSD
* NEXT_WORKSPACE
* PREVIOUS_WORKSPACE

for fine tuned control over 10ftwm. For those curious as to what buttons to use, you can use the "irw" utility to scan for button presses. Here's an example ~/.lircr file:

    begin
    	prog=mceusb
    	button=KEY_OK
    	config=TOGGLE_OSD
    end
    begin
    	prog=mceusb
    	button=KEY_RIGHT
    	config=NEXT_WORKSPACE
    end
    begin
    	prog=mceusb
    	button=KEY_LEFT
    	config=PREVIOUS_WORKSPACE
    end
