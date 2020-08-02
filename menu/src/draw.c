/***************************************************************************/
void Hide_Cursor( void )

{
	_asm mov cx, 1000h;
	_asm mov ah, 01h;
	_asm mov al, video_mode;
	_asm int 10h;
}

/***************************************************************************/
void Show_Cursor( void )

{
	_asm mov ch, cursor_top
	_asm mov cl, cursor_bottom
	_asm mov ah, 01h
	_asm mov al, video_mode
	_asm int 10h
	return;
}

/***************************************************************************/
void Put_Cursor(col,row)

byte col,row;

{
	_asm mov ah,02
	_asm sub bh,bh
	_asm mov dh,row
	_asm mov dl,col
	_asm int 10h
	return;
}

/***************************************************************************/
void Hide_Mouse( void )

{
	_asm mov ax,2
	_asm int 33h
	return;
}

/***************************************************************************/
void Show_Mouse( void )

{
	_asm mov ax,1
	_asm int 33h
	return;
}

/***************************************************************************/
void Draw(x,y,thing,color)

byte x,y,color;
char thing;

{
	*(video_start+x+max_screen_x*y) = (int) ( (byte) thing| (int) (color <<8) );
}

/***************************************************************************/
byte Read_Color(x,y)

byte x,y;

{
	return( *(video_start+x+max_screen_x*y) >> 8);
}

/***************************************************************************/
void Put_Color(x,y,color)

byte x,y,color;

{
	*(video_start+x+max_screen_x*y) = (*(video_start+x+max_screen_x*y) & 255) | (color << 8);
}

/***************************************************************************/
void Fill_Screen(thing,color)

char thing;
byte color;

{
	register int x, fill_value;

	fill_value = ( (byte) thing | color << 8);

	for ( x = max_screen_x * (max_screen_y+1); x>=0 ;x--)
		*(video_start+x) = fill_value;
}

/***************************************************************************/
void Display_Backdrop(void)

{
	FILE *Backdrop;
	char buffer[MAX_LINE];

	Fill_Screen(backdrop_char,backdrop_color);

	if (backdrop_file[0] != '\0'){
		if ( ( Backdrop = fopen(backdrop_file, "r" ) ) != NULL ){
			while ( fgets( buffer, MAX_LINE, Backdrop) != NULL )
				fprintf(stderr,"%s",buffer);
			  fclose(Backdrop);
		}
	}
}

// SBOX has single lines.
// DBOX has double lines
// HBOX has double vertical lines, single horizontal lines.
// IBOX has single vertical lines, double horizontal lines.
// UL Upper-left
// UR Upper-right
// LL Lower-left
// LR Lower-right
// H  Horizontal-line.
// V  Vertical-line.
// TU 'T'-up
// TD 'T'-down
// TL 'T'-left
// TR 'T'-right
// X   Lines cross.

#define SBOX_UL 0xDA
#define SBOX_UR 0xBF
#define SBOX_LL 0xC0
#define SBOX_LR 0xD9
#define SBOX_H  0xC4
#define SBOX_V  0xB3
#define SBOX_TU 0xC1
#define SBOX_TD 0xC2
#define SBOX_TL 0xB4
#define SBOX_TR 0xC3
#define SBOX_X  0xC5

#define DBOX_UL 0xC9
#define DBOX_UR 0xBB
#define DBOX_LL 0xC8
#define DBOX_LR 0xBC
#define DBOX_H  0xCD
#define DBOX_V  0xBA
#define DBOX_TU 0xCA
#define DBOX_TD 0xCB
#define DBOX_TL 0xB9
#define DBOX_TR 0xCC
#define DBOX_X  0xCD

#define HBOX_UL 0xD6
#define HBOX_UR 0xB7
#define HBOX_LL 0xD3
#define HBOX_LR 0xBD
#define HBOX_TU 0xCF
#define HBOX_TD 0xD1
#define HBOX_TL 0xB6
#define HBOX_TR 0xC7
#define HBOX_X  0xD7

#define IBOX_UL 0xD5
#define IBOX_UR 0xB8
#define IBOX_LL 0xD4
#define IBOX_LR 0xBF
#define IBOX_TU 0xCF
#define IBOX_TD 0xD2
#define IBOX_TL 0xB6
#define IBOX_TR 0xC6
#define IBOX_X  0xD8
/***************************************************************************/
void Display_Header()

{
	byte x;
	byte back   = palette[header_palette][ BACK ] << 4;
	byte text   = palette[header_palette][ TEXT ] | back;
	byte border = palette[header_palette][ HIGHLIGHT ] | back;

	if ( header_message == NULL )
		header_message = VERSION_STRING;

		/* draw top */
	x=0;
	Draw(x, 0, SBOX_UL, border);
	for (x++; x < (byte)(max_screen_x-1); x++)
		Draw(x, 0, SBOX_H, border);
	Draw(x, 0,SBOX_UR, border);

		/* draw filling */
	x=0;
	Draw( x++ , 1, SBOX_V, border);
	Draw( x++ , 1, ' ', text);
	while (header_message[x-2] != '\0'){
		Draw(x,1,header_message[x-2],text);
		x++;
	}
	while (x< (byte)(max_screen_x-1) )
		Draw(x++, 1, ' ', text);
	Draw(x, 1, SBOX_V, border);

		/* draw bottom */
	x = 0;
	Draw(x, 2, SBOX_LL, border);
	for(x++; x < (byte)(max_screen_x-1); x++)
		Draw(x, 2, SBOX_H, border);
	Draw(x, 2, SBOX_LR, border);

	Update_Time();
}

/***************************************************************************/
void Display_Footer()

{
	register byte x,i,ch;

	x=0;
	for ( i=1; i < 11 ; i++){
		if (special_item[i] != NULL){
			Draw (x++,max_screen_y,' ',footer_color);
			Draw (x++,max_screen_y,'F',footer_color);
			if ( i == 10 ){
				Draw (x++,max_screen_y,'1',footer_color);
				Draw (x++,max_screen_y,'0',footer_color);
			}
			else
				Draw (x++,max_screen_y,(byte)(i+48),footer_color);
			Draw (x++,max_screen_y,'=',footer_color);
			Draw (x++,max_screen_y,' ',footer_color);
			ch =0;
			while ( special_item[i]->title[ch] != '\0' )
				Draw (x++,max_screen_y,special_item[i]->title[ch++],footer_color);
			Draw (x++,max_screen_y,' ',footer_color);
			special_item[i]->item_number = x;
		}
	}
	if (x > 0){
		while (x < max_screen_x)
			Draw (x++,max_screen_y,' ',footer_color);
	}
}

/***************************************************************************/
void Screen_Saver()

{
    char i;

    current_minute = 61;
    shadow =  NO;    /* turn shadow drawing off */

             /* wait until movement resets the timer */
        while (timer == 0){  
        Update_Time();
        Update_Mouse();
        if (kbhit()){
            Get_Key_Input();
        }
        clock_place++;  
            /* increment clock_place -  this is used to simulate a
               a random placement of the screen saver message 
              by moding this number with the number of possible 
              places to put it out.  This is done in update_time() */
	}

    Hide_Mouse();         /* just to be safe */

    i=win_index;        /* remember how many windows were open */
                        /* draw back boxes will foul this number */
  
	Display_Backdrop();    /* lay screen foundation */
	Display_Header();
	Display_Footer();
    Display_Back_Boxes();

    win_index = 1;      /* begin at the window #1 */
    Last_Window_Globals();    /* go back to window 0 */

                        /* if more windows were open, display them */
	if (do_return){
        while( win_index < i ){  
            Display_Menu( current_item );
            win_index +=2;           /* two steps forward -- */
            Last_Window_Globals();     /* one step back -- what a kludge */
        }
    }
        /* turn shadow drawing back on */
    shadow=draw_shadow;

    current_minute = 61;
    event.action = NO_ACTION;

        /* current window will be redrawn automaticly upon return */ 
}

/***************************************************************************/
int *Store_Screen()

{
    int *storage_addr;
    register int *storage_pointer;
    register byte x,y;

    if ( (storage_addr = (int *) malloc( (width+1) * (height+1) *2 )) == NULL){
        Critical_Error(MEMORY);
    }

    storage_pointer = storage_addr;

    for (y = top; y <= bottom; y++){
        for (x = left; x <= right; x++){
			*storage_pointer = *(video_start+x+max_screen_x*y);
            storage_pointer++;
        }
    }

    return(storage_addr);
}

/***************************************************************************/
void Restore_Screen(storage_addr)

int *storage_addr;

{
    register byte x,y;
    register int *storage_pointer;

    storage_pointer = storage_addr;

    for (y = top; y <= bottom; y++){
        for (x = left; x <= right; x++){
            *(video_start + (max_screen_x*y) +x) = *storage_pointer;
            storage_pointer++;
        }
    }
    free (storage_addr);
    return;
}

/***************************************************************************/
void Display_Menu(hi_item)

Item *hi_item;

{
    byte text,border,title,back;

    register byte x,y,i;
    char *str,quick_number;
    Item *item;

    back  = palette[current_menu->palette][ BACK ] << 4;
    text  = palette[current_menu->palette][ TEXT ] | back;
    title = palette[current_menu->palette][ TITLE ] | back;

    if (hi_item == NULL){    /* draw_active menu */
        if (shadow) Display_Shadow();
        border = palette[current_menu->palette][ HIGHLIGHT ] | back;
		last_x = 0;   /* force mouse reset */
    }
    else{                    /* draw inactive menu */
        if (shadow) Remove_Shadow();
        border = palette[current_menu->palette][ BORDER ] | back;
    }

        /* draw top line */
    x = left;
    y = top;

    Draw(x, y,DBOX_UL, border);
    for (x++; x < right; x++)
        Draw(x, y,DBOX_H, border);
    Draw(x, y, DBOX_UR, border);

		/* draw title centered*/
    x = left;
    y++;
    Draw(x, y, DBOX_V, border);
    for (i = (width-current_menu->title_length)/2; i!=0; i--)
        Draw(++x, y,' ', title);
    for (str = (char *) current_menu->title; *str != '\0'; str++)
        Draw(++x, y, *str, title);
    while ( x < right )
        Draw(++x, y,' ', title);
    Draw(x, y,DBOX_V, border);

        /* draw title bar */
    x = left;
    y++;
    Draw(x, y,DBOX_TR, border);
	for (x++; x < right; x++)
        Draw(x, y,DBOX_H, border);
    Draw(x, y,DBOX_TL, border);

        /* draw body */
    
    item = current_menu->first_item;
    while (item != NULL){
        y++;
        x = left;
        if ( item->first_line != NULL ){
            if (item == hi_item)
                text = palette[current_menu->palette][ HIGHLIGHT ] | back;
            Draw(x, y, DBOX_V, border);
            Draw(++x, y, ' ', text);
            if (quick_select){
										/* make it an asc number */
                quick_number = 48 + item->item_number; 
                if (quick_number > 57)
                                        /* make it an asc letter */
                    quick_number += 7;  
                Draw(++x, y, quick_number, text);
                Draw(++x, y, ')', text);
                Draw(++x, y, ' ', text);
            }                
            for( str = (char *) item->title; *str != '\0'; str++)
                Draw(++x, y, *str, text);
            while( x < right )
                Draw(++x, y, ' ', text);
            Draw(x, y, DBOX_V, border);
            if (item == hi_item) 
                text = palette[current_menu->palette][ TEXT ] | back;
		}
        else{                            /* draw sub-titles centered*/
            x = left;
            Draw(x, y, HBOX_TR, border);
            for (i= (width - item->title_length)/2; i!=0; i--)
                Draw(++x, y, SBOX_H, border);
            for (str = (char *) item->title; *str != '\0'; str++)
                Draw(++x, y, *str, border);
            while (x < right )
                Draw(++x, y, SBOX_H, border);
            Draw(x, y, HBOX_TL, border);
        }
        item = item->next_item;
    }
        /* draw bottom */
    x = left;
	y++;
    Draw(x, y, DBOX_LL, border);
    for(x++; x < right; x++)
        Draw(x, y, DBOX_H, border);
    Draw(x, y, DBOX_LR, border);
}

/***************************************************************************/
void Select(new_item)

/* unselect current_item and then select new_item */

Item *new_item;

{
    register byte x,y;
	byte back,text,select;

    back   = palette[current_menu->palette][ BACK ] << 4;
    text   = palette[current_menu->palette][ TEXT ] | back;
    back   = palette[current_menu->palette][ S_BACK ] << 4;
    select = palette[current_menu->palette][ SELECT ] | back;

    y = top + current_item->item_number + 2;
    x = left + 1;
    while (x < right)
        Put_Color(x++, y, text);

    current_item = new_item;

    y = top + current_item->item_number + 2;
    x = left + 1;
	while ( x < right)
        Put_Color(x++, y, select);

    Win[win_index].item = current_item;
}

/***************************************************************************/
void Display_Text_Box(lines,x1,y1,pal)

char *lines[];
byte x1,y1,pal;

{
    byte text,border,back,x2,y2;
    register byte x,y,i;
    char *str;
	int *storage;

    win_index++;

        /* determine hight and width */
    width=height=0;
    while(lines[height]!=NULL){
        if (width < strlen(lines[height]))        
            width = strlen(lines[height]);
        height++;
    }
    height++;
    width+=3;

    if (x1 == CENTER)
        x1 = (max_screen_x-width)/2;
	if (y1 == CENTER)
        y1 = (max_screen_y-height)/2;

    x2 = x1+width;
    y2 = y1+height;

    if ( x2 >= (byte)(max_screen_x) ){
        x2 = max_screen_x-1;
        x1 = x2 - width;
    }

    if (y2 >= (byte)(max_screen_y) ){
        y2 = max_screen_y-1;
        y1 = y2 - height;
    }

	if ( x1 >= x2 || y1 >= y2 ){
        Last_Window_Globals();
        Error_Box("Unable to display text box.","The box is too large.");
        return;
    }

    left = Win[win_index].left = x1;  
    right = Win[win_index].right = x2;  
    top = Win[win_index].top = y1;  
    bottom = Win[win_index].bottom = y2;  
    Win[win_index].width = width;
    Win[win_index].height = height;
    Win[win_index].storage = Store_Screen();  

    back   = palette[pal][ BACK ] << 4;
    text   = palette[pal][ TEXT ] | back;
	border = palette[pal][ HIGHLIGHT ] | back;

        /* draw top */
    Draw(x = x1, y1, SBOX_UL, border);
    for (x++; x < x2; x++)
        Draw(x, y1, SBOX_H, border);
    Draw(x, y1,SBOX_UR, border);

        /* draw filling */
    for (y=y1+1;y<y2;y++){
        x = x1;
        Draw(x, y, SBOX_V, border);
        str = lines[y-y1-1];
        for( i=( x2-x1-strlen(str)-1 )/2 ;i != 0; i--)
            Draw(++x, y, ' ', text);

		while (*str != '\0'){
            Draw(++x, y, *str, text);
            str++;
        }
        while( x < x2 )
            Draw(++x, y, ' ', text);
        Draw(x, y,SBOX_V, border);
    }

        /* draw bottom */
    Draw(x = x1, y2, SBOX_LL, border);
    for(x++; x < x2; x++)
        Draw(x, y2, SBOX_H, border);
    Draw(x, y2, SBOX_LR, border);

    if (shadow) Display_Shadow();
	last_x = 0;   /* force mouse reset */
}

/***************************************************************************/
void Display_Shadow( )

{
    register byte x, y;

    if (selected_fn) Unselect_Fn();

    right++;
    bottom++;
    for ( x = width+1; x ; x--){
        *(shad_buf+x) = Read_Color( (byte)(left+x), bottom );
        Put_Color( (byte)(left+x), bottom, shadow_color);    
	}
    for (y = height; y ; y--){
        *(shad_buf+max_screen_x+y) = Read_Color(right, (byte)(top+y));
        Put_Color( right, (byte)(top+y), shadow_color);
    }
    right--;
    bottom--;
}

/***************************************************************************/
void Remove_Shadow()

{
    register byte x, y;

    right++;
	bottom++;
    for ( x = width+1; x ; x-- )
        Put_Color( (byte)(left+x), bottom, *(shad_buf+x) );
    for ( y = height; y ; y--)
        Put_Color( right,(byte)(top+y), *(shad_buf+max_screen_x+y) );    
	right--;
	bottom--;
}

/***************************************************************************/
void Clean_Up()

{
	Fill_Screen(' ',7);
	Put_Cursor(0,0);
	Show_Cursor();
	Hide_Mouse();
}

/***************************************************************************/
void Display_Back_Boxes()

{
	char *line[25],*place;
	char temp[MAX_LINE], *checker;
	byte i;

	current_text_block = first_text_block;

	while( current_text_block != NULL ){
		current_text = current_text_block->first_line;
		i=0;
		while ( current_text != NULL && *(place=current_text->text_line) == '"'){
			if (i <25){
				place++;
				Transcribe_String(place,temp);
				checker=temp;
				while (*checker != '\0'){
					if (*checker == '"')
						*checker = '\0';
					checker++;
				}
				line[i] = strdup(temp);
				i++;
		   }
		   current_text = current_text->next_line;
		}
		line[i] = NULL;

		Display_Text_Box(line,current_text_block->x1,current_text_block->y1,current_text_block->pal);
		for (i=0;line[i] != NULL;i++)
			free(line[i]);
		current_text_block = current_text_block->next_block;
	}
}
/***************************************************************************/
void Error_Box(line1,line2)

char line1[],line2[];

{
    char title[] = "Error - Error - Error - Error";
	char foot[] = "Press any key to continue.";
    char blank[] = " ";
    char *lines[7];    

    lines[0] = title;
    lines[1] = blank;
    lines[2] = line1;
	lines[3] = line2;
    lines[4] = blank;
    lines[5] = foot;
    lines[6] = NULL;

    Display_Text_Box(lines,CENTER,CENTER,error_palette);
    Pause_Until_Serious_Action();
    Remove_Window();
}
