/***************************************************************************/
				  /* palettes */
/*BACKground, TEXT, TITLE, BORDER, HIGHLIGHT, Selection BACKground, SELECTion*/

byte palette[ MAX_PALETTES ][ PALETTE_SIZE ] = {
			{1,15,15,15,14,7,1},{3,1,1,1,1,0,15},
			{4,14,14,14,14,7,15},{7,5,1,15,15,5,15},
			{0,3,3,8,7,7,0},{0,12,13,1,9,7,15},
			{0,11,15,8,7,2,15},{2,0,15,10,14,0,10},
			{0,7,15,7,15,7,0},{7,0,0,0,0,0,7}   };

/***************************************************************************/
				/*Global Variables */
#define BACKDROP 0xB1
/* global menu defaults */
byte             default_x = CENTER, default_y = CENTER, default_palette,
				 header_palette, error_palette, message_palette,
				 backdrop_color, footer_color, footer_highlight,
				 backdrop_char = BACKDROP, cascade_x = 4, cascade_y = 1;

boolean          exitable = YES, shadow = NO, draw_shadow = NO, cascade = NO;

/* global pointers */
Menu            *first_menu, *current_menu;
Item            *current_item;
Linked_Text     *current_text;
Linked_Text_Block *first_text_block, *current_text_block;
char            *current_file;
int far         *video_start;
char far        *env_start;

/* working records */
Window_Record   Win[MAX_RECURSION+2];
Event           event;
byte            win_index;

Vars             vars[MAX_VARS];
byte             var_number = 0;

/* working window */
byte            top, bottom, left, right, height, width;

/* file names */
char            *backdrop_file = NULL;

/* special items & their helpers*/
Item            *special_item[MAX_SPECIAL];
	   /*  list of special items :        0       timer
										1 -10   function keys
										11-30   hot keys       */

				/* timer stuff */
byte            current_minute = 61,
				timer = 0, timer_holder = 0;
unsigned int    clock_place;

				/*hot key stuff */
byte            hot_keys = 11;

/* video varibles */
boolean         color_monitor;
byte            max_screen_x, max_screen_y;

/* mouse variables */
byte            last_button, last_x, last_y, selected_fn;

/* shadow stuff */
byte            shadow_color, *shad_buf;

/* other global variables */
byte            l_len;


byte            video_mode;
byte            cursor_top;
byte            cursor_bottom;
boolean	        trap_hard_errors = NO;


unsigned int    line_count;
boolean         show_time = YES;
boolean         first_run = YES;
boolean         do_return = YES;
boolean         quick_select = NO;
boolean         allow_comments = YES;
char            *screen_saver_message = NULL;
char            *header_message = NULL;
