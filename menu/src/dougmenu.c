/***************************************************************************/
/*                Dougmenu for DOS  Feb 11, 1994                           */
/*                                                                         */
/*        This code can be compiled using Borland C++ 3.1                  */
/*          and the netware client API for C kit                           */
/*        A list of all the routines grouped by module can be found        */
/*                         in dougmenu.h                                   */
/***************************************************************************/
#define VERSION_STRING   "DoMenu v1.72"

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <process.h>
#include <dos.h>

#include "dougmenu.h"

#include "globals.c"
#include "setup.c"
#include "novell.c"
#include "parse.c"
#include "draw.c"
#include "input.c"
#include "execute.c"
#include "misc.c"

/***************************************************************************/
void Last_Window_Globals(void)

{
	win_index--;

	current_menu = Win[win_index].menu;
	current_item = Win[win_index].item;
	top    = Win[win_index].top;
	bottom = Win[win_index].bottom;
	left   = Win[win_index].left;
	right  = Win[win_index].right;
	width  = Win[win_index].width;
	height = Win[win_index].height;
}

/***************************************************************************/
boolean Windowfy_Menu(menu)

Menu *menu;

{
	if (menu == NULL){
		current_item = Win[win_index].item;
		return(NO);
	}
	if (win_index == MAX_RECURSION){
		Error_Box("Only 10 windows can open at a time.","The next menu cannot be displayed.");
		current_item = Win[win_index].item;
		return(NO);
	}

	win_index++;
	current_menu = menu;

		/* determine hight and width */
	height = current_menu->number_of_items+3;
	width = current_menu->title_length+3;
	current_item = current_menu->first_item;

	while (current_item != NULL){
		if (quick_select && current_item->first_line != NULL){
			if (width < current_item->title_length+6)
				width = current_item->title_length+6;
		}
		else{
			if (width < current_item->title_length+3)
                width = current_item->title_length+3;
        }
        current_item=current_item->next_item;
    }

    if (height > (byte)(max_screen_y-4) || width >= max_screen_x){
        Last_Window_Globals();
        Error_Box("Unable to display menu.","The menu is too large.");
        return(NO);
    }

        /* determine left,right, top, and bottom */
    left    = current_menu->offset_x;
    top     = current_menu->offset_y;
    if (left == CENTER)
            left   = (byte)( (max_screen_x - width)/2 );
    if (top == CENTER)
            top    = (byte)( (max_screen_y - height)/2 );

    else if (cascade && current_menu->offset_x == Win[0].left &&
             current_menu->offset_y == Win[0].top){
        left   = (byte) ( Win[0].left + (win_index) * cascade_x );
        top    = (byte) ( Win[0].top + (win_index) * cascade_y );
    }    

    if (top <3)
        top = 3;

    right = left + width;
    if (right >= (byte)(max_screen_x-1) ){
        right = (byte)(max_screen_x-2);
        left = (byte)(right - width);
    }

    bottom = top + height;
    if (bottom > (byte)(max_screen_y-1) ){
        bottom = (byte)(max_screen_y-1);
        top = (byte)(bottom - height);
    }

        /* determine if this is a valid menu to display */
    current_item = Home_Item();
    if (current_item == NULL){
        Last_Window_Globals();
        Error_Box("Unable to display menu.","The menu has no choosable options.");
        return(NO);
    }

    Win[win_index].menu    = current_menu;
    Win[win_index].item    = current_item;
    Win[win_index].top     = top;
    Win[win_index].left    = left;
	Win[win_index].right   = right;
	Win[win_index].width   = width;
	Win[win_index].bottom  = bottom;
	Win[win_index].height  = height;
	if (win_index) Win[win_index].storage = Store_Screen();

	return(YES);
}

/***************************************************************************/
Item *Number_To_Item(number)

byte number;

{
	Item *item = current_item;

	if (number > current_menu->number_of_items)
		return ( current_item );

	while (item->item_number < number)
		item = item->next_item;
	while (item->item_number > number)
		item = item->last_item;

	return(item);
}

/***************************************************************************/
Item *Home_Item()

{
	Item *item = current_menu->first_item;

	while (item->first_line == NULL){
		item = item->next_item;
		if (item == NULL)     /* if no selectable items */
			return(NULL);
	}
	return(item);
}

/***************************************************************************/
Item *End_Item()

{
	Item *item = current_item;

	while (item->next_item != NULL)
		item = item->next_item;

	while (item->first_line == NULL)
		item = item->last_item;

	return(item);
}

/***************************************************************************/
Item *Previous_Item()
{
	Item *item = current_item->last_item;

	if (item == NULL)
		return( End_Item() );
	while (item->first_line == NULL){
		if (item->last_item == NULL)
			return( End_Item() );
		item = item->last_item;
	}
	return(item);
}

/***************************************************************************/
Item *Next_Item()
{
	Item *item = current_item->next_item;

	if (item == NULL)
		return( Home_Item () );

	while (item->first_line == NULL){
		if (item->next_item == NULL)
			return( Home_Item () );
		item = item->next_item;
	}
	return(item);
}

/***************************************************************************/
void Remove_Window()

{
	byte number_to_remove=1;
	if (event.action == ESCAPE)
		number_to_remove = event.data;

	if (shadow) Remove_Shadow();

	while ( number_to_remove-- > 0){
		Restore_Screen(Win[win_index].storage);
		Last_Window_Globals();
	}
}

/***************************************************************************/
Menu *Find_Menu(title_text)

Linked_Text *title_text;

{
	char *menu_to_find;
	Menu *menu_to_check = first_menu;


	while ( title_text != NULL && *(title_text->text_line) != '%' ){
		if( title_text->text_line[0]!='#' )
			return(NULL);
		if(Compare(title_text->text_line,"#if"))
			return(NULL);
		title_text = title_text->next_line;
	}
	if (title_text == NULL)
		return(NULL);

	menu_to_find = title_text->text_line;
	menu_to_find++;                              /* chop off '%' */

	while ( strcmpi(menu_to_find,menu_to_check->title) )
		if ((menu_to_check = menu_to_check->next_menu) == NULL){
			Error_Box("Unable to locate menu labeled:",menu_to_find);
			return(NULL);
		}
	return (menu_to_check);
}

/***************************************************************************/
void main( argc, argv )

int argc;
char *argv[];

{
    // When program starts, DS:ES is set to the Program Segment Prefix (PSP).
    // Save the PSP for later before it is overwritten.
    int ds_val = 0;
    int es_val = 0;
    _asm mov ds_val, ds
    _asm mov es_val, es
    saved_psp_segment = ds_val;
    saved_psp_segment <<= 16;
    saved_psp_segment += es_val;



	Break_Off();
	if ( argc < 2 ){
		fputs("Domenu.exe should be called from inside DMENU.BAT\n",stderr);
		exit(LEAVE_MENU);
	}
		// Clear Keyboard Buffer
	while (kbhit())
		getch();

	if (argc == 3)
		Get_Video_Info(argv[2]);
	else
		Get_Video_Info("");

	Fill_Screen(' ',7);
	Set_Up_Enviroment();
	Set_Up_Mouse();
	Parse( argv[1] );

//	current_menu=first_menu;
//	while (current_menu != NULL){
//		puts(current_menu->title);
//		current_item=current_menu->first_item;
//		while (current_item != NULL){
//			printf("  ");
//			puts(current_item->first_line->text_line);
//			current_item=current_item->next_item;
//		}
//		current_menu=current_menu->next_menu;
//		getch();
//	}
//	puts("Done.");
//	getch();

	Set_Up_Screen();


	for (;;){
		Wait_For_Event();
		if (event.action == KEY_PRESS)
			Scan_For_Letter(event.data);

		switch ( event.action ){
			case MOVE_UP:
				Select(Previous_Item());
				break;
			case MOVE_DOWN:
				Select(Next_Item());
				break;
			case GOTO_BOTTOM:
				Select(End_Item());
				break;
			case GOTO_TOP:
				Select(Home_Item());
				break;
			case GOTO_ITEM:
                Select( Number_To_Item( event.data ) );
                break;
            case ESCAPE:
            case BACK_SPACE:
                if (win_index > 0){
                    Remove_Window();
                    Display_Menu(NULL);
                    Select(current_item);
                    break;
                }
                if (!exitable) 
                    break;
                Display_Menu(current_item);
                Exit_Query();
                Display_Menu(NULL);
                Select(current_item);
                if (event.action != SPECIAL)
                    break;

            case DO_ITEM: case SPECIAL:
                Win[win_index].item = current_item;
                Display_Menu( current_item );
                while ( event.action == DO_ITEM || event.action == SPECIAL){
                    Win[win_index].event = DO_ITEM;
                    if (event.action == SPECIAL){
                        current_item = special_item[ event.data ];    
                        Win[win_index].event = SPECIAL+event.data;
                    }
                    Execute_Item();
                }
                current_item = Win[win_index].item;
                Display_Menu( NULL );
                Select( current_item );
                break;
        }
    }
}
