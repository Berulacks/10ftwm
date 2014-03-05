#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>

void setupWindows();
void addWindow( xcb_window_t e );

int main (int argc, char **argv)
{
	uint32_t values[2];
	uint32_t mask = 0;

	//Our connection to the xServer
	xcb_connection_t *connection;
	//Our primary screen
	xcb_screen_t *screen;
	//Our main graphics context
	//So... the background?
	xcb_drawable_t mainGC;

	// #justeventthings
	xcb_generic_event_t *ev;
	//xcb_get_geometry_reply_t *geom;

	xcb_gcontext_t    black;
	xcb_void_cookie_t cookie;

	xcb_generic_error_t *error;

	printf("Starting up!\n");
	
	//Don't have a display name or number,
	//so just go with what xcb gives us.
	connection = xcb_connect( ":1", NULL );

	if( xcb_connection_has_error(connection) )
	{
		printf("Error, could not connect to display!\n");
		return 1;
	}

	screen = xcb_setup_roots_iterator( xcb_get_setup(connection)).data;
	
	printf("Got screen. Width/height: %ix%i\n", screen->width_in_pixels, screen->height_in_pixels);
	

	mainGC = screen->root;	

	xcb_window_t window1;
	window1 = xcb_generate_id( connection );

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_BUTTON_PRESS;

	xcb_create_window( connection, XCB_COPY_FROM_PARENT, window1, mainGC, 0,0 , 150,150 , 14, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);
	xcb_map_window( connection, window1 );

	//black = xcb_generate_id (connection);
	//xcb_create_gc (connection, black, mainGC, XCB_GC_BACKGROUND, &screen->white_pixel);






	xcb_grab_key(connection, 1, mainGC, XCB_MOD_MASK_1, XCB_NO_SYMBOL,
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
		  | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;

	cookie = xcb_change_window_attributes_checked(connection, mainGC, mask, values);
	error = xcb_request_check(connection, cookie);

	if(error != NULL)
	{
		printf("Are you, like, trying to run TWO window managers or something?");
		exit(1);

	}


	xcb_flush(connection);

	for (;;)
	{
		printf("Polling...\n");
		ev = xcb_wait_for_event(connection);
		switch (ev->response_type & ~0x80) 
		{

		case XCB_BUTTON_PRESS:
		{
			printf("Dat button press\n");
		}
		break;

		case XCB_MOTION_NOTIFY:
		{
			printf("Dat motion\n");
		}
		break;
		case XCB_MAP_REQUEST:
		{

			printf("OH MAN I MADE A FRIEND!\n");
			xcb_map_request_event_t *e;

			e = (xcb_map_request_event_t *) ev;
			//addWindow(e->window);
			xcb_window_t win = e->window;
			mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
			values[0] = 10;
			values[1] = 10;

			xcb_configure_window( connection, win, mask, values);
			xcb_map_window(connection, win);


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

	return 0;

}

void addWindow( xcb_window_t e )
{
	

}
