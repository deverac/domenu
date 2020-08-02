#define byte             unsigned char
#define MAX_LINE         140
#define MAX_FILE_NAME    80
#define MAX_VARS         9     
#define MAX_PALETTES     10
#define MAX_SPECIAL      30
#define PALETTE_SIZE     7
#define MAX_RECURSION    9
#define CENTER           255
#define VAR_NOT_FOUND    (char) 255

/***************************************************************************/
typedef enum { NO, YES } boolean;

/***************************************************************************/
typedef enum { EXECUTE_BATCH, LEAVE_MENU, EXECUTE_N_LEAVE } exit_actions;

/***************************************************************************/
typedef enum { BACK, TEXT, TITLE, BORDER, HIGHLIGHT, S_BACK, SELECT } palette_colors;

/***************************************************************************/
typedef enum { MEMORY, FILE_OPEN, SYNTAX, VALUE, POSITION, DISPLAY, 
		ENVIROMENT, PARAM } error_type;

/***************************************************************************/
typedef enum { NO_ACTION, MOVE_UP, MOVE_DOWN, GOTO_TOP, GOTO_BOTTOM, GOTO_ITEM, 
        BACK_SPACE, ESCAPE, KEY_PRESS, OTHER, DO_ITEM, SPECIAL } action_type;

/***************************************************************************/
typedef enum { MAKE_BATCH, NEW_WINDOW, MAKE_TEXT_BOX, GET_DISK_SPACE, 
        FIND_FLOPPY, ASK_FOR_VAR, PASSWORD, LOGOUT, EXIT, REBOOT,
        IF, IF_USERNAME, IF_MEMBER, ELSE, ENDIF }
        execute_option;

/***************************************************************************/
struct linked_text
{
    struct linked_text *next_line;
    char   text_line[1];
};

typedef struct linked_text Linked_Text;

/***************************************************************************/
struct linked_text_block
{
    Linked_Text              *first_line;
    struct linked_text_block *next_block;
    byte                     x1,y1,pal;
};

typedef struct linked_text_block Linked_Text_Block;

/***************************************************************************/
struct item
{
    byte             title_length, item_number;
    struct item      *next_item,*last_item;
    struct linked_text *first_line;
    char             title[1];
};

typedef struct item Item;

/***************************************************************************/
struct menu
{
    byte             offset_x, offset_y, palette, number_of_items, 
                     title_length;
    struct menu      *next_menu;
    struct item      *first_item;
    char             title[1];
};

typedef struct menu Menu;

/***************************************************************************/
struct window_record
{
    Menu            *menu;
    Item            *item;
    int             *storage;
    byte            left, right, width, top, bottom, height;
    action_type     event;
};

typedef struct window_record Window_Record;

/***************************************************************************/
struct event
{
    action_type   action;
	byte          data;
};

typedef struct event Event;

/***************************************************************************/
struct vars
{
    char   *name, *value;
};

typedef struct vars Vars;


/***************************************************************************/


/* procedure definitions */

	/* setup.c */
int 		C_Break_Handler( void );
void        Break_Off( void );
void        Monochrome_Defaults( void );
void        Get_Video_Info( char *parameter );
void        Set_Up_Enviroment( void );
void        Set_Up_Mouse( void );
void        Set_Up_Screen( void );


	/* parse.c */
void		IncludeExist(char *place);
void 		Parse_Engine_Exist(void);
void        Read_Line(char *t_line, FILE *Script );
void        Parse_If( char *place, FILE *Script );
void        Parse_If_Username( char *place, FILE *Script );
void        Parse_If_Member( char *place, FILE *Script );
void        Parse_Else( FILE *Script );
void        Parse_Command(char *place,FILE *Script);
void        Parse_Menu(char t_line[]);
void        Parse_Item(char t_line[]);
void        Parse_Item_Text(char t_line[]);
void        Parse_Engine( void );
void        Parse( char dos_param_2[] );
void        Include( char *place );

	/* draw.c */
void  		Draw(byte x,byte y,char thing,byte color);
byte 		Read_Color(byte x, byte y);
void 		Put_Color(byte x,byte y,byte color);
void 		Fill_Screen(char thing,byte color);
void 		Put_Cursor(byte col,byte row);
void        Hide_Cursor( void );
void        Show_Cursor( void );
void        Hide_Mouse( void );
void        Show_Mouse( void );
void        Clean_Up( void );
void        Screen_Saver( void );
int         *Store_Screen( void );
void        Restore_Screen(int *storage_addr );
void        Display_Menu(Item *hi_item);
void        Select(Item *new_item);
void        Display_Text_Box(char *lines[],byte x1,byte y1, byte pal);
void        Display_Shadow( void );
void        Remove_Shadow( void );
void        Display_Backdrop( void );
void        Display_Header( void );
void        Display_Footer( void );
void        Display_Back_Boxes( void );
void        Error_Box(char line1[],char line2[]);

	/* input.c */
void        Update_Time( void );
void        Select_Fn( byte x );
void        Unselect_Fn( void );
byte        Old_Window(byte x,byte y);
void        Update_Mouse( void );
void        Scan_For_Letter(char key_input );
void        Get_Key_Input( void );
void        Wait_For_Event( void );
void        Pause_Until_Serious_Action( void );

	/* dougmenu.c */
boolean     Windowfy_Menu(Menu *menu);
Item        *Number_To_Item(byte number);
Item        *Home_Item( void );
Item        *End_Item( void );
Item        *Previous_Item( void );
Item        *Next_Item( void );
void        Last_Window_Globals( void );
void        Remove_Window( void );
Menu        *Find_Menu( Linked_Text *title_text );
void        main( int argc, char *argv[] );

	/* execute.c */
byte        Do_Query_Window( Menu *query );
void        Exit_Query( void );
void        Choice_Query(char *place, char temp[] );
void        String_Query(char *place, char string[], boolean show_it );
void        Create_Text_Box( char *place );
void        Get_Disk_Space( char *place );
void        Find_Floppy( char *place );
void        Ask_For_Pass( char *place );
void        Logout( void );
void        Reboot( void );
void        Execute_Item( void );
execute_option What_To_Do( char *temp);
void        Make_Batch( void );
void        Exec_If( char *place );
void        Exec_If_Username( char *place );
void        Exec_If_Member( char *place );
void        Exec_Else( char *place );

	/* misc.c */
Menu        *Allocate_Menu( void );
Item        *Allocate_Item( void );
Linked_Text *Allocate_Linked_Text( void );
int far     Crit_Error_Handler( unsigned deverror, unsigned errcode, unsigned far *devhdr );
long int    Disk_Space_Check( byte drive );
void        Find_Env_Var( char *var, char *string, byte space);
boolean     Fcompare( char *string1,char far *string2 );
void        Ask_For_Var( char *place );
void        Transcribe_String(char *old,char *new);
boolean     Get_DM_Var( char *place, char *temp, byte space);
void        Critical_Error(error_type error);
boolean     Compare(char *string1, char *string2);
char        *After_Space(char *place);
char        *After_Equals(char *place);
char        *After_Comma(char *place);
byte        Get_Palette(char *place, byte def_value);
byte        Get_Color(char *place,byte def_value);
byte        Get_Value(char *place, byte def_value);

/* Holds the Program Segment Prefix */
long int saved_psp_segment = 0;
