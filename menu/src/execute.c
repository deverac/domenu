/***************************************************************************/
byte Do_Query_Window(query)

Menu *query;

{
    byte return_value;

    if ( !Windowfy_Menu( query ) ){
        event.action = ESCAPE;
        event.data = 1;
        return(0);
    }

    Display_Menu(NULL);
    Select(current_item);

    for (;;){
        Wait_For_Event();
        if (event.action == KEY_PRESS)
            Scan_For_Letter(event.data);            

        switch ( event.action ){
            case GOTO_ITEM:
                Select( Number_To_Item( event.data ) );
                break;
            case MOVE_UP:
                Select( Previous_Item());
                break;
            case MOVE_DOWN:
                Select( Next_Item());
                break;
            case GOTO_BOTTOM:
                  Select( End_Item());
                break;
            case GOTO_TOP:
                  Select( Home_Item());
                break;
            case SPECIAL:
            case ESCAPE:
            case DO_ITEM:
    /* this is done this way because Remove window canges current_item */
                return_value = current_item->item_number;
                Remove_Window();
                return( return_value );
        }
    }
}

/***************************************************************************/
void Exit_Query()

{
    Item *no,*yes;
    Menu *exit_query;

    l_len = 12;
    exit_query = Allocate_Menu();
	strcpy(exit_query->title,"Exit Menu?");

	no = Allocate_Item();
	strcpy(no->title,"No");
	yes = Allocate_Item();
	strcpy(yes->title,"Yes");

	exit_query->first_item=yes;
	yes->next_item=no;
	yes->last_item=NULL;
	yes->title_length=3;
	no->last_item=yes;
	no->next_item=NULL;
	no->title_length=2;

			/* make items choosable */
	yes->first_line= current_item->first_line;
	no->first_line= current_item->first_line;
	yes->item_number=1;
	no->item_number=2;

	exit_query->number_of_items = 2;
	exit_query->title_length    = 10;
	exit_query->offset_x = CENTER;
	exit_query->offset_y = CENTER;
	exit_query->palette  = message_palette;

	if ( Do_Query_Window(exit_query) == 1 && event.action == DO_ITEM){
		Clean_Up();
		exit(LEAVE_MENU);
	}

	free(exit_query);
	free(no);
	free(yes);
}

/**************************************************************************/
void Choice_Query(place,temp)

char *place,temp[];

{
    Menu *choice_query;
    Item *item=NULL;
    Linked_Text *line = current_text;
    byte i, value;
     
    l_len = MAX_LINE;
    choice_query = Allocate_Menu();

    for (i = 0; *place != ',' && *place!= '\0';i++)
        choice_query->title[i] = *(place++);
    choice_query->title[i] = '\0';
    choice_query->title_length = strlen(choice_query->title);

    if (*place != '\0') place++;
    choice_query->offset_x = Get_Value(place,default_x);
    place = After_Comma(place);
    choice_query->offset_y = Get_Value(place,default_y);
    place = After_Comma(place);
    choice_query->palette = Get_Palette(place,current_menu->palette);
    
    line = line->next_line;

    choice_query->number_of_items = 0;

    while (line != NULL && line->text_line[0] == '"'){
        if (item == NULL){
            item = Allocate_Item();
            choice_query->first_item = item;
            item->last_item = NULL;
            item->next_item = NULL;
        }
        else{
            item->next_item = Allocate_Item();
            item->next_item->last_item = item;
            item = item->next_item;
            item->next_item = NULL;
        }

            /* read option from quotes */
        for (i=0; line->text_line[i+1] != '"' && line->text_line[i+1] != '\0'; i++)
            item->title[i] = line->text_line[i+1];
        item->title[i] = '\0';
        item->title_length = i;
        item->item_number = ++choice_query->number_of_items;

            /* make it a chooseable option if it should be done */
        item->first_line = NULL;    
        while ( line->text_line[++i] != '\0' )
            if ( line->text_line[i] == '=' )
                item->first_line = line;    

        line = line->next_line;
    }


    value = Do_Query_Window(choice_query);

    while (item->last_item != NULL){
        item = item->last_item;
        free (item->next_item);
    }
    free(item);
    free(choice_query);

    if ( event.action != DO_ITEM ){
        return;
    }

    line = current_text;              /* find chosen line */
    for (;value>0;value--){
        line = line->next_line;
    }

    strcpy( temp, After_Equals(line->text_line) );
}

/**************************************************************************/
void String_Query(place,string, show_it)

char *place,string[];
boolean show_it;

{
    Menu *string_query;
    Item *space_holder;
    byte x,y,length=0,i=0,max_length,color;
    boolean holder = quick_select;
    quick_select = NO;

    if (show_it)
        max_length = 56;
    else 
        max_length = 36;

    l_len = 10;
    space_holder = Allocate_Item();
    space_holder->first_line = current_item->first_line;
    space_holder->title_length = max_length;
    space_holder->next_item = NULL;
    strcpy(space_holder->title," ");

    l_len = 60;
    string_query = Allocate_Menu();
    for (i = 0; *place != ',' && *place!= '\0';i++){
        string_query->title[i] = *(place++);
        if ( i >= 58 )
            break;
    }
    string_query->title[i] = '\0';
    place = After_Comma(place);
    string_query->offset_x = Get_Value(place, CENTER );
    place = After_Comma(place);
    string_query->offset_y = Get_Value(place,(byte)( (max_screen_y/2) +4) );
    place = After_Comma(place);
    string_query->palette = Get_Palette(place,current_menu->palette);

    string_query->title_length = i;
    string_query->number_of_items = 1;
    string_query->first_item = space_holder;

    if (!Windowfy_Menu(string_query)){
        free(space_holder);
        free(string_query);
        quick_select = holder;
        return;
    }

    Display_Menu(NULL);

    x = (byte)(left+2);
    y = (byte)(top+3);
    color = (byte)(palette[string_query->palette][TEXT] | palette[string_query->palette][BACK]<<4);
    string[0] = '\0';
    Show_Cursor();

    for(;;){
        Put_Cursor( (byte) (x+length) ,y);
        Wait_For_Event();

        switch (event.action){
            case KEY_PRESS:
                if (length < max_length && event.data ){                
                    string[length] = event.data;
                    if (show_it)
                        Draw( (byte)(x+length),y,event.data,color);
                    else
                        Draw( (byte)(x+length),y,'X',color);
                    length++;
                    string[length] = '\0';
                }
                break;
            case BACK_SPACE:
                if (length){
                    length--;
                    string[length] = '\0';
                    Draw( (byte)(x+length),y,' ',color);
                }
                break;
            case ESCAPE:
            case DO_ITEM:
            case SPECIAL:
                 Hide_Cursor();
                Remove_Window();
                free(space_holder);
                free(string_query);
                quick_select = holder;
                return;
        }
    }
}

/**************************************************************************/
void Ask_For_Var(place)

char *place;
{
    char temp_string[MAX_LINE];
    byte i;

        /* find var.name and record it */
    place = After_Space(place);
    for (i=1; *place != ' ' && *place != ',' && *place != '\0'; i++ )
        temp_string[i] = *(place++);
    temp_string[0] = '%';
    temp_string[i] = '%';
    temp_string[++i] = '\0';
    vars[var_number].name = strdup(temp_string);
    place = After_Comma(place);

                 /* get value in temp_string[] */
    if (current_text->next_line->text_line[0] == '"') 
        Choice_Query(place,temp_string);
    else 
        String_Query(place,temp_string,YES);

                 /* copy it into var.value */
    vars[var_number].value = strdup(temp_string);
    var_number++;
}

/**************************************************************************/
void Ask_For_Pass(place)

char *place;

{
    Menu *pass_query;
    char user_input[MAX_LINE];    
    char title[] = "Please enter password:";

    place = After_Space(place);

    String_Query(title,user_input,NO);

    if ( event.action == DO_ITEM && stricmp(place,user_input) ) {
        Error_Box("This password is not authorized.","Action aborted.");
        if (event.action != SPECIAL){
            event.action = ESCAPE;
            event.data = 1;
        }
    }
}

/**************************************************************************/
void Create_Text_Box(place)

char *place;

{
    char *line[25];
    char temp[MAX_LINE], *checker;
    Linked_Text *l_line = current_text;
    byte x,y,color,i=0;

    place = After_Comma(place);
    x     = Get_Value(place, CENTER );
    place = After_Comma(place);
    y     = Get_Value(place, CENTER );
    place = After_Comma(place);
    color = Get_Palette(place,message_palette);

    l_line = l_line->next_line;

    while ( l_line != NULL && *(place=l_line->text_line) == '"'){
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
        l_line = l_line->next_line;
    }
    line[i] = NULL;

    Display_Text_Box(line,x,y, color);
    Pause_Until_Serious_Action();
    Remove_Window();

    for (i=0;line[i] != NULL;i++)
        free(line[i]);
}    
/**************************************************************************/
void Get_Disk_Space(place)

char *place;

{
     long int minimum = 0;
    byte drive = 0;
    char line1[MAX_LINE],line2[MAX_LINE];

    place = After_Space(place);

    if (*(place+1)==':' && *(place+2) == ' ')
        drive = *place & 31;
    else{
        Error_Box("Now that's a wierd drive lable.","I can't look there.");
        event.action = ESCAPE;
        event.data = 1;
        return;
    }

    place = After_Space(place);

    while ( (*place >= '0' && *place <= '9') || *place == ',' || *place == '.'){
        if ( *place != ',' && *place != '.')
            minimum = ( minimum * 10 ) + *place - 48;
        place++;
    }

    if ( Disk_Space_Check(drive) < minimum*1000 ){
        sprintf( line1 , "This option requires %lu K bytes",minimum);
        sprintf( line2 , "of free disk space on drive %c:.",drive+64);
        Error_Box( line1, line2 );
        event.action = ESCAPE;
        event.data = 1;
    }
}

/**************************************************************************/
void Find_Floppy(place)

char *place;

{
    char temp[MAX_LINE];
    byte f_drives,i;

    char *lines1[] = {"This Application Requires a User's Diskette.","",
                "** Please Insert a Formatted Diskette. **",
                "","Press Any Key When Ready,","or Escape to Exit.",NULL};

    char *lines2[] = {"Looking for a Formatted Diskette.","",
                "Please wait.",NULL};

    Display_Text_Box(lines1,CENTER,CENTER,current_menu->palette);
    Pause_Until_Serious_Action();
    Remove_Window();

    if (event.action == ESCAPE || event.action == SPECIAL)
        return;

    place = After_Space(place);

    for (i=1; *place != ' ' && *place != ',' && *place != '\0'; i++ )
        temp[i] = *(place++);
    temp[0] = '%';
    temp[i] = '%';
    temp[++i] = '\0';

        /* find number of floppies */
    f_drives = *((byte far *) 0x00400010L);
    f_drives = (f_drives & 1 ) + (f_drives >> 6);

        /* check for floppy */
    Display_Text_Box(lines2,CENTER,CENTER,current_menu->palette);

    for (i = 1; i <= f_drives; i++){
        if ( Disk_Space_Check(i) != -1 )
            break;
    }
    Remove_Window();

    if (i > f_drives){
        Error_Box("No Formated Floppy Diskette Was Found.","Action Aborted.");
        if (event.action != SPECIAL){
            event.action = ESCAPE;
            event.data = 1;
        }
        return;
    }
                /* store var name */
    vars[var_number].name = strdup(temp);
                 /* copy drive name into var.value */
    temp[0] = (char)i+64;
    temp[1] = ':';
    temp[2] = '\0';
    vars[var_number].value = strdup(temp);
    var_number++;
}

/**************************************************************************/
void Logout( void )

{
    _asm mov ah,0d7h
    _asm int 21h
}

/**************************************************************************/
void Reboot( void )

{
    typedef void (far *Pointer_to_Bios_Fn)( void );
    Pointer_to_Bios_Fn reboot;

    Clean_Up();

    reboot = (Pointer_to_Bios_Fn)0xFFFF0000;
        /* store magic number to cause warm boot */
    *(int far *)0x00400072 = 0x1234;
    (*reboot)();
}

/**************************************************************************/
void Execute_Item()

{
    char temp[MAX_LINE];

    if (current_item == NULL){
        Screen_Saver();
        return;
    }

    current_text = current_item->first_line;
    event.action = NO_ACTION;

    while (current_text != NULL && event.action != ESCAPE && event.action != SPECIAL){
        Transcribe_String(current_text->text_line,temp);
        switch ( What_To_Do(temp) ){
        case ASK_FOR_VAR:
            Ask_For_Var(temp);
                break;
            case FIND_FLOPPY:
                Find_Floppy(temp);
                break;
            case GET_DISK_SPACE:
                Get_Disk_Space(temp);
                break;
            case LOGOUT:
                Logout();
                break;
            case MAKE_TEXT_BOX:
                Create_Text_Box(temp);
                break;
            case PASSWORD:
                Ask_For_Pass(temp);
                break;
            case MAKE_BATCH:
                Make_Batch();
                break;
            case NEW_WINDOW:
                Windowfy_Menu( Find_Menu( current_text ) );
                event.action=ESCAPE;
                break;
            case IF:
                Exec_If(temp);
                break;
            case IF_MEMBER:
                Exec_If_Member(temp);
                break;
            case IF_USERNAME:
                Exec_If_Username(temp);
                break;
            case ELSE:
                Exec_Else(temp);
                break;
            case ENDIF:
                break;
            case EXIT:
                Clean_Up();
                exit(LEAVE_MENU);
            case REBOOT:
                Reboot();
        }
        if (var_number > MAX_VARS)
            Critical_Error( MEMORY );
        do{
            current_text = current_text->next_line;
            if (current_text == NULL)
                break;
        }
        while (current_text->text_line[0] == '"');
    }
    while (var_number){
        var_number--;
        free(vars[var_number].name);
        free(vars[var_number].value);
    }
    return;        
}
                    
/**************************************************************************/
execute_option What_To_Do(temp)

char *temp;

{
    if ( temp[0] == '%')
        return(NEW_WINDOW);
    if ( Compare(temp,"#display") )
        return(MAKE_TEXT_BOX);
    if ( Compare(temp,"#space ") )
        return(GET_DISK_SPACE);
    if ( Compare(temp,"#floppy ") )
        return(FIND_FLOPPY);
    if ( Compare(temp,"#ask ") )
        return(ASK_FOR_VAR);
    if ( Compare(temp,"#password ") )
        return(PASSWORD);
    if ( Compare(temp,"#logout") )
        return(LOGOUT);
    if ( Compare(temp,"#reboot") )
        return(REBOOT);
    if ( Compare(temp,"#exit") )
        return(EXIT);
    if ( Compare(temp,"#if ") )
        return(IF);
    if ( Compare(temp,"#endif") )
        return(ENDIF);
    if ( Compare(temp,"#if_member") )
		return(IF_MEMBER);
    if ( Compare(temp,"#if_username") )
        return(IF_USERNAME);
    if ( Compare(temp,"#else") )
        return(ELSE);

    return(MAKE_BATCH);
}

/**************************************************************************/
void Make_Batch()

{
    char temp[MAX_LINE];
	char *place;
	FILE *Temp_Bat;
	byte i;

	Clean_Up();

	//current_menu=first_menu;
	//while (current_menu != NULL){
	//	puts(current_menu->title);
	//	current_item=current_menu->first_item;
	//	while (current_item != NULL){
	//		printf("  ");
	//		puts(current_item->first_line->text_line);
	//		current_item=current_item->next_item;
	//	}
	//	current_menu=current_menu->next_menu;
	//	getch();
	//}
	//puts("Done.");
	//getch();

			/* open file to write */
	Find_Env_Var("%t%",temp,MAX_LINE);
	if ( ( Temp_Bat = fopen(temp, "w" ) ) == NULL )
		Critical_Error(FILE_OPEN);
			/* write file header */
	 fputs("@REM   Temporary file for DoMenu :",Temp_Bat);
	for (i=0;i<=win_index;i++){
		fputc(Win[i].item->item_number+64,Temp_Bat);
		fputc(Win[i].event+64,Temp_Bat);
    }
    fputc('\n',Temp_Bat);

            /* Write the batch file */
    while (current_text != NULL){
        place = current_text->text_line;
        Transcribe_String(place,temp);
        switch (What_To_Do(place)){
            case IF:
                Exec_If(temp);
                break;
            case IF_MEMBER:
                Exec_If_Member(temp);
                break;
            case IF_USERNAME:
                Exec_If_Username(temp);
                break;
            case ELSE:
                Exec_Else(temp);
                break;
            case ENDIF:
                break;
            case EXIT:
                fputs("set t=",Temp_Bat);
                fclose (Temp_Bat);
                exit(EXECUTE_N_LEAVE);
            default:
                /* check for variable names */
                while (*place != '\0'){
                    while (*place == '%'){
                        if ( !Get_DM_Var(place,temp,MAX_LINE)){
                            break;
                        }
                        fputs (temp,Temp_Bat);
                        place++;
                        while (*place != '%')
							place++;
                        place++;
                    }
                    if (*place != '\0'){
                        fputc(*place,Temp_Bat);
                        place++;
                    }
                }
                fputc('\n',Temp_Bat);
        }
        current_text = current_text->next_line;
    }    
    fclose (Temp_Bat);
    exit(EXECUTE_BATCH);
}

/***************************************************************************/
void Exec_If( place )

char *place;

{
    char *var1,*var2;
    byte i=0;
    
    var2 = var1 = After_Space(place);

    
    while ( *var2 != '!' && *var2 != '=' && *var2 != '\0' ){
        var2++;
    }
         /* trim spaces from end of var1 */
    if (*var2 != '\0'){
        while (*(var2-1)== ' ')
            *(--var2)='\0';
        while (*var2== '\0')
            var2++;
    }

    if ( *var2 == '!'){
        if ( *(var2+1) != '=' )
            Critical_Error(SYNTAX);
        *var2 = '\0';
        var2 += 2;
        while (*var2 == ' ')
            var2++;
        if (stricmp(var1,var2))
            return;
    }            
    else if ( *var2 == '='){
        *var2 = '\0';
        var2++;
        if ( *(var2) == '=' )
            var2++;
        while (*var2 == ' ')
            var2++;
        if (!stricmp(var1,var2)){
            return;
        }
    }
    else if ( *var1 != '%' && *var1 != '\0' ){
        return;
    }

    for(i=0;;){
        current_text = current_text->next_line;
        if (current_text == NULL )
            return;
        place = current_text->text_line;

        if (Compare(place,"#endif"))
            if (!i--)
                return;
        if (Compare(place,"#else"))
            if (!i)
                return;
        if (Compare(place,"#if "))
            i++;
        if (Compare(place,"#if_member"))
            i++;
        if (Compare(place,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Exec_If_Member( place )

char *place;

{
	char *var1;
	byte i=0;
	
	var1 = After_Space(place);

	if (*var1 == '!'){
        var1++;
        if (!IsMember (var1))
            return;
    }
    else
        if (IsMember (var1))
            return;

    for(i=0;;){
        current_text = current_text->next_line;
        if (current_text == NULL )
            return;
        place = current_text->text_line;

        if (Compare(place,"#endif"))
            if (!i--)
                return;
        if (Compare(place,"#else"))
            if (!i)
                return;
        if (Compare(place,"#if "))
            i++;
		if (Compare(place,"#if_member"))
			i++;
		if (Compare(place,"#if_group"))
			i++;
	}
}

/***************************************************************************/
void Exec_If_Username( place )

char *place;

{
	char *var1;
	byte i=0;

	var1 = After_Space(place);

	if (*var1 == '!'){
		var1++;
		if (!UserName(var1))
			return;
	}
	else
		if (UserName(var1))
			return;

	for(i=0;;){
		current_text = current_text->next_line;
		if (current_text == NULL )
			return;
		place = current_text->text_line;

		if (Compare(place,"#endif"))
			if (!i--)
				return;
        if (Compare(place,"#else"))
            if (!i)
                return;
        if (Compare(place,"#if "))
            i++;
        if (Compare(place,"#if_member"))
            i++;
        if (Compare(place,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Exec_Else( place )

char *place;

{
    byte i=0;
    
    for(i=0;;){
        current_text = current_text->next_line;
        if (current_text == NULL )
            return;
        place = current_text->text_line;

        if (Compare(place,"#endif"))
            if (!i--)
                return;
        if (Compare(place,"#else"))
            if (!i)
                return;
        if (Compare(place,"#if "))
            i++;
        if (Compare(place,"#if_member"))
            i++;
        if (Compare(place,"#if_group"))
            i++;
    }
}
