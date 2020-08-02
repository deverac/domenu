/**************************************************************************/
int C_Break_Handler( void )

{
	return(1);
		// returning a value of 1 allows the program execution to continue
}


/**************************************************************************/
void Break_Off(void)

{
	freopen("NUL:","w",stdout);
			/* capture stdout to stop ^c from apearing */
//	ctrlbrk( C_Break_Handler );
}

/****************************************************************************/
void Monochrome_Defaults()

{
	 color_monitor    = NO;
	 default_palette  = 8;
	 header_palette   = 9;
	 error_palette    = 9;
	 message_palette  = 9;
	 backdrop_color   = 7;
	footer_color     = 15;
	footer_highlight = 4;
}

/***************************************************************************/
void Get_Video_Info( parameter )

char *parameter;

{
	// color defaults
	color_monitor    = YES;
	video_start = (int far * ) 0xb8000000;
	default_palette  = 0;
	header_palette   = 1;
	error_palette    = 2;
	message_palette  = 3;
	backdrop_color   = 19;
	footer_color     = 31;
	footer_highlight = 113;

	// put the screen to the 0 video page of whatever mode we are in.
	_asm mov ax, 0500h
	_asm int 10h

	// record the cursor shape
	_asm mov ah,03
	_asm mov bh,00    //look in page 0
	_asm int 10h
	_asm mov cursor_top,ch
	_asm mov cursor_bottom,cl

	if (!stricmp(parameter,"mono")){
		max_screen_x = 80;
		max_screen_y = 24;
		video_start=(int far * ) 0xb0000000;
		Monochrome_Defaults();
	}
	else if (!stricmp(parameter,"color")){
		max_screen_x = 80;
		max_screen_y = 24;
		video_start=(int far * ) 0xb8000000;
	}
	else if (stricmp(parameter,""))
		Critical_Error(PARAM);

	// find out what video mode we are in.
	_asm mov ax, 0F00h
	_asm int 10h
	_asm mov video_mode,al;
	if (video_mode >= 7){
		// monochrome defaults
		video_start = (int far * ) 0xb0000000;
		color_monitor    = NO;
		default_palette  = 8;
		header_palette   = 9;
		error_palette    = 9;
		message_palette  = 9;
		backdrop_color   = 7;
		footer_color     = 15;
		footer_highlight = 4;
	}
	// now determine the screen size
	max_screen_y = *((char far *) 0x00400084L);
	max_screen_x = *((char far *) 0x0040004AL);
	if (max_screen_y == 0){
		max_screen_x = 80;
		max_screen_y = 24;
	}
}

/***************************************************************************/
void Set_Up_Enviroment()

/* this routine changes the enviroment var's as needed and gets the path
  of the temporary batch file from there */

{
	long int psp_segment;
	long int env_segment;

	int far *ptr;
	char far *cptr;

	psp_segment=saved_psp_segment;

		/* go to the oldest enviroment space */

	while (ptr !=(ptr = (int far *) ((psp_segment << 16) + 0x16)));
	psp_segment = *ptr;
	ptr = (int far *) ((psp_segment << 16) + 0x2c);
	env_segment = *ptr;
	cptr = env_start = (char far *) (env_segment <<16);

    while (!Fcompare( "%t%", cptr )){
		while (*cptr != '\0')
			cptr++;
		if (*(++cptr) == '\0'){
			Critical_Error(ENVIROMENT);
		}
	}

	while (*cptr != '\0')
		cptr++;
	cptr--;

	if (*(cptr-3) != '.')
		Critical_Error(ENVIROMENT);
	if (*cptr == 'T') 
		first_run = NO;

	else{
		first_run = YES;
		*cptr-- = 'T';
		*cptr-- = 'A';
		*cptr = 'B';
   }
}

/***************************************************************************/
void Set_Up_Mouse()

{
	int max_x,max_y;

	max_x = (max_screen_x -1 ) * 8;
	max_y = max_screen_y * 8;

	_asm sub ax,ax           /* intitalize mouse */
	_asm int 33h

	_asm mov dx,max_x    /* Pixels horizontally */
	_asm mov ax,7        /* mouse driver function 7 -- set max x range */
	_asm sub cx,cx       /* Minimum range */
	_asm int 33h

	_asm mov dx,max_y    /* Pixels veritcally */
	_asm mov ax,8        /* mouse driver function 8 -- set max y range */
	_asm sub cx,cx       /* Minimum range */
	_asm int 33h

	_asm mov ax,4        /* set current mouse position  0,0 */
	_asm sub bx,bx
	_asm sub cx,cx
	_asm sub dx,dx
	_asm int 33h

	Hide_Mouse();
}

/***************************************************************************/
void Set_Up_Screen()

{
	char    ch;
	char 	old_bat[MAX_FILE_NAME];
	FILE   *Old_Bat;

	Hide_Cursor();
	Display_Backdrop();
	Display_Header();
	Display_Footer();
    Display_Back_Boxes();

	win_index = 255;         /* we are now at the -1st menu */

                             /* this sets win[].item */
	if (!Windowfy_Menu(first_menu))
		Critical_Error(DISPLAY);

	if ( !first_run ){
		Find_Env_Var("%t%",old_bat,MAX_LINE);
		if ( ( Old_Bat = fopen(old_bat, "r" ) ) != NULL ){
			if ( do_return ){
					/* advance to the pertinent information */
				while ( (ch = fgetc(Old_Bat)) != ':' )
				{
					if (feof(Old_Bat))
						break;
					if ( ch == '\n')
						break;
				}
				for (;;){
                       // get item number
					ch = (char) (fgetc(Old_Bat) - 64);
					current_item = Number_To_Item( (byte)ch );
					if (current_item->first_line == NULL)
						break;
					Win[win_index].item = current_item;
					Display_Menu(current_item);

                       // do it if possible
					ch = (char) (fgetc(Old_Bat) - 64);
                       // now 42, that's a magic number */
					if ( (byte)ch > SPECIAL+30 )
						break;
					Win[win_index].event = (byte)ch;
					if ( ch != DO_ITEM)
						current_item = special_item[ch-SPECIAL];
					if ( current_item == NULL )
						break;
					if ( !Windowfy_Menu( Find_Menu( current_item->first_line ) ) )
						break;
				}
			}
			fclose(Old_Bat);
			remove(old_bat);
		}
	}

		/* start doing shadows now */
		/* allocate the shadow buffer if necessary */
    if ( (shadow=draw_shadow)!=0 ){
		shad_buf = (byte *) malloc ( max_screen_x + max_screen_y );
	}

		/*make sure that the current item is proper */
	current_item = Win[win_index].item;
	Display_Menu(NULL);
	Select(current_item);
}
