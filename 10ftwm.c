#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <xcb/xcb.h>

#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include <stdbool.h>

#include <linux/joystick.h>

#define OSD_FONT "12x24"
//#define OSD_FONT "-urw-urw palladio l-medium-r-normal--0-0-0-0-p-0-iso8859-16"
#define OSD_H_W 100

#define L_SHIFT 40
#define UP 111
#define R_ARROW 114
#define L_ARROW 113

#define GP_TOTAL_KEYS 10

#define GP_KEY_A 0
#define GP_KEY_B 1
#define GP_KEY_HOME 8


/* --- List stuff --- */
typedef struct node
{
	int data;
	struct node* next;
} linkedList;

linkedList* createList(int data);
int sizeOfList(linkedList head);
int getFromList(linkedList head, int index);
void appendToList(linkedList* head, int data);
void removeFromList(linkedList* head, int index);
int indexOf( linkedList list, int data );
void printList ( linkedList list );
/* --- End of List stuff --- */

void setupWindows();
void updateCurrentWindow(int index);
void addWindow( xcb_window_t e );
void toggleOSD();
int loop();

//Init
void processInput(int argc, char **argv);
void readFromFileAndConfigure(char* filename);

int joystick;
bool hasJoystick;

int gpKeyLastPressed[ GP_TOTAL_KEYS ];;

int screen_number;
char* displayName;

//Our primary screen
xcb_screen_t* screen;
//Our connection to the xServer
xcb_connection_t *connection;
//Our current window (always at front)
int currentWindowIndex;

//A list of windows
linkedList windowList;

xcb_window_t osd;
bool osdActive;
xcb_font_t osdFont;
xcb_gcontext_t osdGC;

void processInput(int argc, char **argv)
{
	int opt;

	displayName = NULL;
	screen_number = 0;

	while ((opt = getopt(argc, argv, "hs:d:")) != -1) 
	{
		switch (opt) 
		{
		case 's':
			printf("Opening on display %s...\n", optarg);
			displayName = optarg;
			break;
		case 'd':
			printf("Opening on screen number %i...\n", atoi(optarg));
			screen_number = atoi(optarg);
			break;
		case 'h':
			printf("\
			\n10ftwm: A lightweight window manager designed to be used with media centers and other '10 foot' devices.\
			\nUsage: 10ftwm -d [DISPLAY NAME] -s [SCREEN NUMBER]\
			\nDISPAY NAME:\
			\nThe display to open. Default is the contents of your $DISPLAY variable.\
			\nSCREEN NUMBER:\
			\nThe screen number to open. Default is 0.\n");

			exit(0);
			break;
	       }
	}
}


void readFromFileAndConfigure(char* filename)
{

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("Couldn't open file!\n");
		return;
	}


	while ((read = getline(&line, &len, fp)) != -1) {
	   printf("Retrieved line of length %zu, and size %zu :\n", read, len);
	   printf("%s", line);
	   //if(line[0] = "screen")
		//printf("DETECTED SCREEN\n");
	}

	if (line)
	   free(line);

	fclose(fp);
}

int main (int argc, char **argv)
{
	uint32_t values[3];
	uint32_t mask = 0;
	osdActive = false;

	for(int i = 0; i < GP_TOTAL_KEYS; i++)
		gpKeyLastPressed[i] = 0;

	//Our main graphics context
	//So... the background?
	xcb_drawable_t mainGC;


	xcb_void_cookie_t cookie;

	xcb_generic_error_t *error;

	processInput(argc, argv);
	readFromFileAndConfigure("10ftwmrc");
	
	//Don't have a display name or number,
	//so just go with what xcb gives us.
	connection = xcb_connect(displayName, &screen_number);

	if( xcb_connection_has_error(connection) )
	{
		printf("Error, could not connect to display!\n");
		return 1;
	}

	screen = xcb_setup_roots_iterator( xcb_get_setup(connection)).data;
	
	printf("Got screen. Width/height: %ix%i\n", screen->width_in_pixels, screen->height_in_pixels);
	

	mainGC = screen->root;	

	//Get osd id
	osd = xcb_generate_id( connection );
	//Font id
	osdFont = xcb_generate_id( connection );
	//Open font
	char* fontName = OSD_FONT;	
	cookie = xcb_open_font(connection, osdFont, strlen(fontName), fontName);
	error = xcb_request_check (connection, cookie);
	if (error) 
	{
		printf ("ERROR: can't open font : %d\n", error->error_code);
	}
	else
		printf("Loaded font '%s'.\n", fontName);


	//OSD values
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_BUTTON_PRESS;

	//Create OSD
	xcb_create_window( connection, XCB_COPY_FROM_PARENT, osd, mainGC, 0,0 , OSD_H_W,OSD_H_W , 14, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);

	osdGC = xcb_generate_id(connection);
	
	mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
	values[0] = screen->black_pixel;
	values[1] = screen->white_pixel;
	values[2] = osdFont;
	xcb_create_gc (connection, osdGC, osd, mask, values);

	
	//Grab keys/mouse buttons
	xcb_grab_key(connection, 1, mainGC, XCB_MOD_MASK_SHIFT, R_ARROW,
		 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

	xcb_grab_key(connection, 1, mainGC, XCB_MOD_MASK_SHIFT, L_ARROW,
		 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

	xcb_grab_key(connection, 1, mainGC, XCB_MOD_MASK_SHIFT, UP,
		 XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);

	xcb_grab_button(connection, 0, mainGC, XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
		XCB_GRAB_MODE_ASYNC, mainGC, XCB_NONE, 1, XCB_MOD_MASK_1);

	xcb_grab_button(connection, 0, mainGC, XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
		XCB_GRAB_MODE_ASYNC, mainGC, XCB_NONE, 3, XCB_MOD_MASK_1);

	/* Subscribe to events. */
	mask = XCB_CW_EVENT_MASK;

	values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
		  | XCB_EVENT_MASK_STRUCTURE_NOTIFY
		  | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
		  | XCB_EVENT_MASK_KEY_PRESS;

	cookie = xcb_change_window_attributes_checked(connection, mainGC, mask, values);
	error = xcb_request_check(connection, cookie);

	if(error != NULL)
	{
		printf("Are you, like, trying to run TWO window managers or something?\n");
		exit(1);

	}


	xcb_flush(connection);


	hasJoystick = false;

	joystick = open ("/dev/input/js0", O_RDONLY | O_NONBLOCK);
	if(joystick >= 0)
	{
		printf("Found joystick!\n");
		hasJoystick = true;
	}
	else
		printf("Could not find joystick :(\n");


	int looping = 1;

	while(looping)
		looping = loop();
			


	return 0;

}

int loop()
{

		//Variables for
		//select()
		int fd;                        
		fd_set in;                    

		//Get X's file descriptor, we already have
		//the joystick's one
		fd = xcb_get_file_descriptor(connection);
 
		int max = joystick > fd ? joystick : fd;

		FD_ZERO(&in);
		FD_SET(fd, &in);
		FD_SET(joystick, &in);
		select(max+1, &in, NULL, NULL, NULL);


		// #justeventthings
		xcb_generic_event_t *ev;

		if( FD_ISSET(fd, &in) )
			while( (ev = xcb_poll_for_event(connection)) )
			{
				switch (ev->response_type & ~0x80) 
				{

					case XCB_BUTTON_PRESS:
					{
						printf("Dat button press\n");
						return 0;
					}
					break;

					case XCB_KEY_PRESS:
					{
						xcb_key_press_event_t* e;
						e = (xcb_key_press_event_t*) ev;
						printf("You pressed %i\n", e->detail);

						if(e->detail == R_ARROW)
							updateCurrentWindow(currentWindowIndex+1);
						if(e->detail == L_ARROW)
							updateCurrentWindow(currentWindowIndex-1);
						if(e->detail == UP)
							toggleOSD();
				}
				break;

				case XCB_DESTROY_NOTIFY:
				{
					xcb_destroy_notify_event_t *e;
					e = (xcb_destroy_notify_event_t *) ev;

					printf("Received window destroy notification for window %i.\n", e->window);

					int index = indexOf( windowList, e->window ); 
					if(index == -1)
					{
						printf("	Could not find window %i in list.\n", e->window);
						break;
					}

					removeFromList( &windowList, index);
					int numWindows = sizeOfList(windowList);
					printf("Removed window %i from list. There are now a total of %i windows.\n", index, sizeOfList(windowList) );

					if(currentWindowIndex >= numWindows)
					       currentWindowIndex = numWindows-1; 	
					else
						if(currentWindowIndex < 0)
							currentWindowIndex = 0;

				}
				break;

				case XCB_MAP_REQUEST:
				{

					xcb_map_request_event_t *e;
					e = (xcb_map_request_event_t *) ev;

					printf("Received map request for window %i.\n", e->window);
					addWindow(e->window);

				}
				break;

				case XCB_BUTTON_RELEASE:
				{
				    xcb_ungrab_pointer(connection, XCB_CURRENT_TIME);
				    xcb_flush(connection);
				}
				break;

				}
			}

		if(hasJoystick && FD_ISSET(joystick, &in) )
		{
			struct js_event event;
			//Reading...
			read(joystick, &event, sizeof(event));
			//...books!

			if(event.type == JS_EVENT_BUTTON)
			{
				//If button press
				if(event.value == 1 && gpKeyLastPressed[ event.number ] == 0)
				{
					printf("You pressed button %i on the gamepad!\n", event.number);
					//Only allow controller functions while the OSD is active
					//...this does not apply to the Home key, which is responsible
					//for toggling the OSD
					if(osdActive || event.number == GP_KEY_HOME)
						switch(event.number)
						{
							case GP_KEY_A:
								updateCurrentWindow(currentWindowIndex+1);
								break;
							case GP_KEY_B:
								updateCurrentWindow(currentWindowIndex-1);
								break;
							case GP_KEY_HOME:
								toggleOSD();
								break;
						}
					gpKeyLastPressed[ event.number ] = 1;
				}
				//If button release
				else
					if(event.value != 1 && gpKeyLastPressed[ event.number ] == 1)
					{
					printf("You released button %i on the gamepad!\n", event.number);
					gpKeyLastPressed[ event.number ] = 0;
					}
					
			}
		}


		return 1;

}

void updateCurrentWindow(int index)
{
	//Sanity check to see if we have any windows
	//to begin with
	if(!sizeOfList(windowList))
		return;

	currentWindowIndex = index;
	if(currentWindowIndex >= sizeOfList(windowList) )
		currentWindowIndex = 0;
	if(currentWindowIndex < 0)
		currentWindowIndex = sizeOfList(windowList)-1;

	printf("Bringing window %i to front\n", currentWindowIndex);

	uint32_t values[] = { XCB_STACK_MODE_TOP_IF };
	xcb_configure_window (connection, getFromList( windowList, currentWindowIndex ), XCB_CONFIG_WINDOW_STACK_MODE, values);

	if( osdActive)
		xcb_configure_window (connection, osd, XCB_CONFIG_WINDOW_STACK_MODE, values);

	//This only works for numbers 0-9, should be changed in the future!
	char curPos = (char)(((int)'0')+currentWindowIndex);
	xcb_image_text_8(connection, sizeof(curPos), osd, osdGC, OSD_H_W/2, OSD_H_W/2, &curPos);
		
	xcb_flush(connection);
}

void toggleOSD()
{
	if( !osdActive )
	{
		xcb_map_window(connection, osd);
		osdActive = true;
		uint32_t values[] = { XCB_STACK_MODE_TOP_IF };
		xcb_configure_window (connection, osd, XCB_CONFIG_WINDOW_STACK_MODE, values);
		
		//This only works for numbers 0-9, should be changed in the future!
		char curPos = (char)(((int)'0')+currentWindowIndex);
		xcb_image_text_8(connection, sizeof(curPos), osd, osdGC, OSD_H_W/2, OSD_H_W/2, &curPos);

		printf("OSD enabled.\n");
	}
	else
	{
		xcb_unmap_window(connection, osd);
		osdActive = false;
		printf("OSD disabled.\n");
	}

	xcb_flush(connection);
}

void addWindow( xcb_window_t window )
{
	uint32_t mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
	uint32_t values[2];
	xcb_void_cookie_t cookie;
	xcb_generic_error_t *error;

	values[0] = screen->width_in_pixels;
	values[1] = screen->height_in_pixels;

	xcb_configure_window( connection, window, mask, values);
	cookie = xcb_map_window(connection, window);
	appendToList(&windowList, window);

	if( osdActive)
	{
		//If our osd is supposed to be active,
		//draw it on top of the current window
		uint32_t value[] = { XCB_STACK_MODE_TOP_IF };
		xcb_configure_window (connection, osd, XCB_CONFIG_WINDOW_STACK_MODE, value);
	}

	error = xcb_request_check(connection, cookie);
	if(error != NULL)
		printf("Something went wrong while adding a window!\n");
	else
		xcb_flush(connection);

	printf("Added window %i into position %i. There are now a total of %i windows.\n", window, indexOf( windowList, window ), sizeOfList(windowList) );
	updateCurrentWindow( indexOf( windowList, window ) );


}

/* --- LIST STUFF --- */
linkedList* createList(int data)
{
	linkedList* x = malloc( sizeof( linkedList ) );
	x->data = data;
	x->next = NULL;

	return x;
}

int sizeOfList(linkedList head)
{
	if(isnan(head.data) || isinf(head.data) )
		return 0;
	int size = 0;
	linkedList* x = &head;	

	while(x->next != NULL)
	{
		size++;
		x = x->next;
	}

	return size;
}

int getFromList(linkedList head, int index)
{
	if(index >= sizeOfList(head))
	{
		printf("ERROR: index out of range\n");
		//This will always break something since we only ever
		//use lists with unsigned ints.
		return -1;
	}

	linkedList* x = &head;

	for(int i = 0; i <= index; i++)
		x = x->next;

	return x->data;
}

void appendToList(linkedList* head, int data)
{
	linkedList* x = malloc( sizeof(linkedList) );
	x->data = data;
	x->next = NULL;

	if(head->next == NULL)
	{
		head->next = x;
	}
	else
	{
		linkedList* itr = head;
		for(int i = 0; i < sizeOfList(*head); i++)
			itr = itr->next;
		itr->next = x;
	}
	
}

void removeFromList(linkedList* head, int index)
{
	int size = sizeOfList(*head);
	if(size == 0)
		return;
	if( size == 1 )
	{
		head->next = NULL;
		return;
	}

	linkedList* x = head;

	//If we're at the end of the list
	//then stop iterating at the
	//penultimate member
	if(index == size-1)
		index--;

	for( int i = 0; i <= index; i++)
		x = x->next;

	linkedList* toDie = x->next;
	x->next = toDie->next;
	
	toDie->next = NULL;
	free(toDie);
}

int indexOf( linkedList list, int data )
{
	int index;
	int size = sizeOfList(list);
	linkedList* x = &list;
	//printf("***\nSearching for match for %i...\n", data);

	for(index = 0; index < size; index++)
	{
		x = x->next;
		if(x->data == data)
			return index;
	}	
	printf("[NOTICE]: Could not find index of data.\n");
	return -1;

}

void printList ( linkedList list )
{
	int size = sizeOfList(list);
	printf("List has size of %i, and contains the data:\n", size);
	for(int i = 0; i < size; i++)
		printf("[ %i ]\n", getFromList(list, i) );
}
/* --- END OF LIST STUFF --- */
