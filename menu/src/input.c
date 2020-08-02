/***************************************************************************/
void Update_Time()

{
     char time_str[35];
    char *message;
      
    byte minutes,hours,day_o_week,day_o_month,month,am_pm,i,time_len,color;
    byte x,y,max_time_str = 34;

    _asm mov ah,2ch
    _asm int 21h
    _asm mov minutes,cl
    _asm mov hours,ch

     if (current_minute != minutes){

        current_minute = minutes;
         _asm mov ah,2ah        
        _asm int 21h
        _asm mov day_o_week,al
        _asm mov day_o_month,dl
        _asm mov month,dh

        am_pm = 0;
         if (hours >= 12) {
            am_pm = 1;
            hours -=12;
        }
        if (hours == 0)
            hours = 12; 
            
        switch (day_o_week){
            case  0:
                strcpy(time_str,"Sunday, ");
                break;
            case  1:
                strcpy(time_str,"Monday, ");
                break;
            case  2:
                strcpy(time_str,"Tuesday, ");
                break;
            case  3:
                strcpy(time_str,"Wednesday, ");
                break;
            case  4:
                strcpy(time_str,"Thursday, ");
                break;
            case  5:
                strcpy(time_str,"Friday, ");
                break;
            case  6:
                strcpy(time_str,"Saturday, ");
                break;
        }
        switch (month){
            case  1:
                strcat( time_str,"January");
                break;
            case  2:
                strcat( time_str,"February");
                break;
            case  3:
                strcat( time_str,"March");
                break;
            case  4:
                strcat( time_str,"April");
                break;
            case  5:
                strcat( time_str,"May");
                break;
            case  6:
                strcat( time_str,"June");
                break;
            case  7:
                strcat( time_str,"July");
                break;
            case  8:
                strcat( time_str,"August");
                break;
            case  9:
                strcat( time_str,"September");
                break;
            case  10:
                strcat( time_str,"October");
                break;
            case  11:
                strcat( time_str,"November");
                break;
            case  12:
                strcat( time_str,"December");
                break;
        }

                /* day, hours,  minutes and am/pm */
        sprintf( (time_str + strlen(time_str))," %d, %d:%02d ",day_o_month,hours,minutes);

        if (am_pm) strcat( time_str,"pm");
        else strcat(time_str,"am");

                /* display time */
        time_len = strlen(time_str);
        message = time_str;
        color = (byte) (palette[header_palette][BACK] <<4) | (palette[header_palette][TEXT]);

            /* tigger timer special action if time expires */
        if (timer){
            if (!--timer){
                event.action = SPECIAL;
                event.data = 0;
            }    
        }
          // Determine where and if to display the time
        x = max_screen_x -2;
        y = 1;
        if ( !timer && timer_holder ){
            Fill_Screen(' ',0);
            if (screen_saver_message != NULL)
                message = screen_saver_message;

            max_time_str = time_len = strlen( message );
                /* invent a psuedo random number */
            x = time_len + clock_place % (max_screen_x - time_len);
            y = clock_place % max_screen_y;
        }
        else if (!show_time)
			return;

        Hide_Mouse();
        for (i=1;i <= time_len ;i++)
            Draw( (byte)(x-i),y,*(message+time_len-i),color);

            /* overlap old spot should new time be shorter */
        for (;i<max_time_str;i++)
            Draw( (byte)(x-i),y,' ',color);

        Show_Mouse();
    }
}

/***************************************************************************/
void Unselect_Fn()

{
    byte x,i;

    if (!selected_fn)
        return;

    i = selected_fn;

    for (i--;i>0;i--)
        if (special_item[i] != NULL)
            break;

    if (!i) x=0;
    else x=special_item[i]->item_number;

    Hide_Mouse();
    for (;x < special_item[selected_fn]->item_number;x++)
        Put_Color(x,max_screen_y,footer_color);
    Show_Mouse();    

    selected_fn = 0;    
}

/***************************************************************************/
void Select_Fn(x)

byte x;

{
    byte i;

    for (i=1;i<11;i++)
        if (special_item[i]!=NULL && special_item[i]->item_number>x)
            break;        

    if (i == selected_fn)
        return;

    if (i == 11){
        Unselect_Fn();
        return;
    }

    Unselect_Fn();

    selected_fn=i;
    for (i--;i>0 && special_item[i] == NULL;i--);

    if (!i) x=0;
    else x=special_item[i]->item_number;

    Hide_Mouse();
    for (;x< special_item[selected_fn]->item_number;x++)
        Put_Color(x,max_screen_y,footer_highlight);
    Show_Mouse();
}    

/***************************************************************************/
byte Old_Window(x,y)

byte x,y;

{
    byte window = (byte) win_index;

    for (window--;window!=255;window--)
        if (x>=Win[window].left && x<= Win[window].right &&
                y>=Win[window].top && y<= Win[window].bottom)
            return( (int) win_index - window);
    return( 0);
}

/***************************************************************************/
void Update_Mouse()

{
    unsigned int mouse_x,mouse_y,button;
    byte choice;
    Item *item;

    _asm mov ax,3        /* read current mouse position */
    _asm sub bx,bx            
    _asm sub cx,cx            
    _asm sub dx,dx
    _asm int 33h
    _asm mov mouse_x,cx
    _asm mov mouse_y,dx
    _asm mov button,bx

    mouse_x = mouse_x >> 3;
    mouse_y = mouse_y >> 3;

            /* decide what is going on with the mouse */

    if (mouse_x == 0 && mouse_y == 0)
        return;

    if (mouse_x == last_x && mouse_y == last_y && button == last_button)
        return;

        /* select item if the mouse is over a selectable item */

    choice = mouse_y-top-2;

    if ( mouse_x>left && mouse_x<right ){
        if ( choice>0 && choice<=current_menu->number_of_items){
            if ( choice!=current_item->item_number ){
                item = Number_To_Item( (byte) choice );
                if (item->first_line != NULL){
                    event.action = GOTO_ITEM;
                    event.data = choice;
                  }
              }
         }
        /*  this checks for top & bottom run off */
        else if ( last_y < bottom  && last_y > (byte)top+2 ){ 
            if ( last_y > mouse_y) 
                choice = 1;
            else
                choice = current_menu->number_of_items;
            item = Number_To_Item( (byte) choice );
            if (item->first_line != NULL){
                event.action = GOTO_ITEM;
                event.data = choice;
              }
        }
    }

        /*  highlight footer spots */
    if (mouse_y == max_screen_y )
        Select_Fn( (byte)mouse_x);
    else if (selected_fn)
        Unselect_Fn();

        /* left button */
    if ( !(last_button & 1) && (button & 1)){
        event.action = OTHER;
        if ( mouse_x>left && mouse_x<right && choice>0 && choice<=current_menu->number_of_items ){
            item = Number_To_Item( (byte) choice );
            if (item->first_line != NULL)
                event.action = DO_ITEM;
        }
        else if (selected_fn){
            event.action = SPECIAL;
            event.data = selected_fn;
        }        
        else if ( mouse_x < left || mouse_x > right || mouse_y > bottom || mouse_y < top){ 
            choice = Old_Window( (byte)mouse_x, (byte)mouse_y );
            if (choice > 0){
                event.action = ESCAPE;
                event.data = choice;
            }
        }
    }
    
        /* right button */
    if ( !(last_button & 2) && (button & 2) ){
        event.action = ESCAPE;
        event.data = 1;
    }

    timer = timer_holder;   /* reset timer */

    last_x      = mouse_x;
    last_y      = mouse_y;
    last_button = button;

    Show_Mouse();
}

/***************************************************************************/
void Get_Key_Input()

{
    char key_input;
    byte index;

    timer = timer_holder;

    key_input = getch();

    event.action = KEY_PRESS;
    event.data = key_input;

    if (!key_input)     /* if key input == 0 get the extended part */
        switch(key_input = getch()){
            case 71:   /* home */
            case 73:   /* page up */
                event.action = GOTO_TOP;
                return;
            case 72:   /* up arrow */
                event.action = MOVE_UP;
                return;
            case 75:   /* back arrow */
            case 83:   /* delete key */
                event.action = BACK_SPACE;
                event.data = 1;
                return;
            case 79:   /* end */
            case 81:   /* page down */
                event.action = GOTO_BOTTOM;
                return;
            case 80:   /* down arrow */
                event.action = MOVE_DOWN;
                return;
            default:
                    /* chech fn keys */
                if (key_input >58 && key_input < 69){
                    key_input -= 58;
                    if (special_item[key_input] != NULL){
                        event.action = SPECIAL;
                        event.data = key_input;
                    }
                    return;
                }            
                    /* check extended hot keys */
                for(index=11;index < hot_keys;index++){
                    if (special_item[index]->item_number == 0 && special_item[index]->title_length == key_input){
                        event.action = SPECIAL;
                        event.data = index;
                        return;
                    }
                }
                event.action = NO_ACTION;
                return;
        }
    switch(key_input){
        case 8:
            event.action = BACK_SPACE;
            event.data = 1;
            return;
        case 13:
            event.action = DO_ITEM;
            return;
        case 27:
            event.action = ESCAPE;
            event.data = 1;
            return;
        default:
                /* check regular hot keys */
            for(index=11;index < hot_keys;index++){
                if (special_item[index]->item_number == key_input){
                    event.action = SPECIAL;
                    event.data = index;
                    return;
                }
            }
            return;
    }
}

/***************************************************************************/
void Scan_For_Letter( key_input )

char key_input;

{
    Item *item = current_item->next_item;

    if (key_input == ' '){
        event.action = MOVE_DOWN;
        return;
    }

    key_input = key_input | 32;  /* ignore case */

    if (quick_select){
        if ( key_input > 48 && key_input <= 57 ){
            if ( key_input <= (byte) (current_menu->number_of_items + 48) ){
                item = Number_To_Item( (byte) (key_input - 48) );
                if ( item->first_line != NULL ){
                    Win[win_index].item = current_item = item;
                    event.action = DO_ITEM;
                }
            }
        }
        if ( key_input > 96  ){
                    /* 87 is becuase 'a' is actually 10 */
            if ( key_input <= (byte) (current_menu->number_of_items + 87 ) ){
                item = Number_To_Item( (byte) (key_input - 87) );
                if ( item->first_line != NULL ){
                    Win[win_index].item = current_item = item;
                    event.action = DO_ITEM;
                }
            }
        }
        return;
    }                

    for (; item != NULL; item = item->next_item)
        if (item->first_line != NULL && (*(item->title)|32) == key_input){
            event.action = GOTO_ITEM;
            event.data = item->item_number;
            return;
        }
    for (item = Home_Item(); item != current_item; item = item->next_item)
        if (item->first_line != NULL && (*(item->title)|32) == key_input){
            event.action = GOTO_ITEM;
            event.data = item->item_number;
            return;
          }
}            

/***************************************************************************/
void Wait_For_Event()

{
    Show_Mouse();
 
    event.action = NO_ACTION;

    while( event.action == NO_ACTION ){
        Update_Mouse();
        Update_Time();
        if (kbhit())
            Get_Key_Input();
    }

    Hide_Mouse();
}
/***************************************************************************/

void Pause_Until_Serious_Action( void )

{
    do
        Wait_For_Event();
    while (event.action == GOTO_ITEM);

    if (event.action != SPECIAL && event.action != ESCAPE)
        event.action = NO_ACTION;
}
