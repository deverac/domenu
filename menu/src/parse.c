
/***************************************************************************/
/* This routine reads a line of input while :
        changing tabs to spaces
        preventing excessive spaces at the begining of the line
        preventing a string overflow by ingoring anything over MAX_LINE
        removing dangling spaces from the end of the line
        ending the line with a null character instead of a \n or EOF
            unless the only char in the string is an EOF
        ignoring comments
        also increments line_count & sets l_len */

void Read_Line(t_line, Script )

char *t_line;
FILE *Script;

{
    byte inquotes = 0;

	l_len = 0;
	*t_line = getc(Script);

    if (feof(Script))
        return;

    while ( *t_line != '\n'){
					/* ignore ;'s and all that follows if not in quotes */
		if (*t_line == '"')
			inquotes = ~inquotes;
		else if (!inquotes && *t_line == ';' && allow_comments ){
			while ( *t_line != '\n') {
				if (feof(Script))
					break;
				*t_line = getc(Script);
            }
			break;
		}
					 /* change tabs to spaces */
		if (*t_line == '\t')
			*t_line = ' ';
					/* don't let spaces build up at front */
		if (l_len != 1 || *t_line != ' ' || *(t_line-1) != ' '){
					/* don't make string too long */
			if (l_len < MAX_LINE){
				l_len++;
				t_line++;
			}
		}
		*t_line = getc(Script);
        if (feof(Script))
            break;
	}
					/* remove spaces from the end */
	while ( *(t_line-1) == ' ' && l_len != 0 ){
		l_len--;
		t_line--;
	}
	*t_line = '\0';
	line_count++;
}


/***************************************************************************/
void Parse_If( place, Script )

char *place;
FILE *Script;

{
    char temp_line[MAX_LINE];
    char *var1,*var2;
    byte i=0;
    
	Transcribe_String(place,temp_line);
    var2 = var1 = After_Space(temp_line);

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
	else if ( *var1 != '%' ){
        return;
    }
    else if ( *var1 == '\0')
        Critical_Error(SYNTAX);

    for(i=0;;){
        Read_Line(temp_line, Script );
        if (feof(Script))
            return;
        if (Compare(temp_line,"#endif"))
            if (!i--)
				return;
        if (Compare(temp_line,"#else"))
            if (!i)
                return;
        if (Compare(temp_line,"#if "))
            i++;
        if (Compare(temp_line,"#if_member"))
            i++;
        if (Compare(temp_line,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Parse_If_Member( place, Script )

char *place;
FILE *Script;

{
	char temp_line[MAX_LINE];
	char *var1;
	byte i=0;
	
	Transcribe_String(place,temp_line);
	var1 = After_Space(temp_line);

	if (*var1 == '!'){
        var1++;
        if (!IsMember(var1))
            return;
	}
    else 
        if (IsMember(var1))
            return;

	for(i=0;;){
        Read_Line(temp_line, Script );
        if (feof(Script))
            return;
        if (Compare(temp_line,"#endif"))
            if (!i--)
                return;
        if (Compare(temp_line,"#else"))
            if (!i)
                return;
        if (Compare(temp_line,"#if "))
            i++;
		if (Compare(temp_line,"#if_member"))
            i++;
        if (Compare(temp_line,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Parse_If_Username( place, Script )

char *place;
FILE *Script;

{
	char temp_line[MAX_LINE];
	char *var1;
	byte i=0;

	Transcribe_String(place,temp_line);
	var1 = After_Space(temp_line);

	if (*var1 == '!'){
		  var1++;
		  if ( !UserName(var1) )
			  return;
	}
	else
		if ( UserName(var1) )
			return;

	for(i=0;;){
		Read_Line(temp_line, Script );
        if (feof(Script))
			return;
		if (Compare(temp_line,"#endif"))
			if (!i--)
				return;
        if (Compare(temp_line,"#else"))
            if (!i)
                return;
        if (Compare(temp_line,"#if "))
            i++;
        if (Compare(temp_line,"#if_member"))
            i++;
        if (Compare(temp_line,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Parse_Else( Script )

FILE *Script;

{
    char temp_line[MAX_LINE];
    byte i=0;
    
    for(i=0;;){
        Read_Line(temp_line, Script );
        if (feof(Script))
            return;
        if (Compare(temp_line,"#endif"))
            if (!i--)
                return;
        if (Compare(temp_line,"#else"))
            if (!i)
                return;
        if (Compare(temp_line,"#if "))
            i++;
        if (Compare(temp_line,"#if_member"))
            i++;
		if (Compare(temp_line,"#if_group"))
            i++;
    }
}

/***************************************************************************/
void Include( place )

char *place;

{
    char included_script[ MAX_FILE_NAME + 4 ];
	int count_holder = line_count;
    char *file_holder = current_file;

    strncpy(included_script,After_Equals(place),MAX_FILE_NAME);
    if (included_script[0] == '\0')
        Critical_Error(VALUE);
    line_count = 0;
    current_file = included_script;    
    Parse_Engine();
    current_file = file_holder;    
    line_count = count_holder;
}

/***************************************************************************/
void IncludeExist( place )

char *place;

{
	char included_script[ MAX_FILE_NAME + 4 ];
	int count_holder = line_count;
	char *file_holder = current_file;

	strncpy(included_script,After_Equals(place),MAX_FILE_NAME);
	if (included_script[0] == '\0')
		Critical_Error(VALUE);
	line_count = 0;
	current_file = included_script;	
	Parse_Engine_Exist();
	current_file = file_holder;
	line_count = count_holder;
}


/***************************************************************************/
void Parse_Command(place, Script)

char *place;
FILE *Script;

{
    byte index,i;

    place++;                                    /* trim '#' */

    if (Compare(place,"if ")){
		Parse_If(place, Script);
        return;
	}

	if (Compare(place,"if_member ")){
		Parse_If_Member(place, Script);
		return;
	}

	if (Compare(place,"if_username ")){
		Parse_If_Username(place, Script);
		return;
	}

	if (Compare(place,"else")){
		Parse_Else(Script);
		return;
	}

	if (Compare(place,"endif"))
		return;

    if (Compare(place,"palette")){
        index = Get_Palette(place,0);
        place=After_Equals(place);        
        for (i = 0; i < PALETTE_SIZE; i++){
			palette[index][i] = Get_Value(place,palette[index][i]);
            place = After_Comma(place);
        }
        return;
    }

    if (Compare(place,"timer")){
        timer = Get_Value(place,15);    /* 15 minute default */
        timer_holder = ++timer;
        
        l_len = 1;   /* don't save room for title, 'cause it has none */
        special_item[0] = Allocate_Item();
		current_item = special_item[0];
        current_text = NULL;
        current_item->item_number = 0;
        current_item->first_line = NULL;
        return;
    }

    if (Compare(place,"saver ")){
        place = After_Space(place);
        if (Compare(place,"message")){
            place = After_Equals(place);
            if (*place != '\0'){
				screen_saver_message = (char *) malloc(MAX_LINE);
                Transcribe_String(place,screen_saver_message);
                return;
            }            
            Critical_Error(VALUE);
        }
        Critical_Error(SYNTAX);
    }
    
    if (Compare(place,"f") && *(place+1) <= '9' && *(place+1) >= '0'){
        index = Get_Value(place,0);
        if (index > 10 || index == 0)
			Critical_Error(VALUE);
        l_len -= 3;

        current_item = special_item[index] = Allocate_Item();
        current_text = current_item->first_line = NULL;

        strncpy(current_item->title,After_Equals(place),l_len);
        return;
    }

    if (Compare(place,"key")){
        l_len = 3;

        current_item = special_item[hot_keys] = Allocate_Item();
        current_text = current_item->first_line = NULL;

            /* key scan code */
        current_item->item_number = Get_Value(place,0);
        place = After_Comma(place);
			/* extended key scan code */
		current_item->title_length = Get_Value(place,0);

        if (++hot_keys > MAX_SPECIAL)
            Critical_Error(MEMORY);

        return;
    }
    if (Compare(place,"footer ")){
		place = After_Space(place);
		if (Compare(place,"color")){
			footer_color = Get_Color( place, footer_color );
			return;
		}
		if (Compare(place,"highlight")){
			footer_highlight = Get_Color( place, footer_highlight );
			return;
		}
		Critical_Error(SYNTAX);
	 }

	if (Compare(place,"include_if_exist")){
		IncludeExist(place);
		return;
	}

	if (Compare(place,"include")){
		Include(place);
		return;
	}

	if (Compare(place,"message ")){
		place = After_Space(place);
		if (Compare(place,"palette")){
			message_palette = Get_Palette(place,message_palette);
			return;
		}
		Critical_Error(SYNTAX);
	}

	if (Compare(place,"error ")){
		place = After_Space(place);
		if (Compare(place,"palette")){
			error_palette = Get_Palette(place,error_palette);
			return;
		}
		Critical_Error(SYNTAX);
	}
	if (Compare(place,"display")){
		if (current_text_block == NULL)
			first_text_block = current_text_block =
				 (Linked_Text_Block *) malloc (sizeof (Linked_Text_Block));
        else{
			current_text_block->next_block =
                 (Linked_Text_Block *) malloc(sizeof(Linked_Text_Block));
            current_text_block = current_text_block->next_block;
        }
        if (current_text_block == NULL)
             Critical_Error(MEMORY);

             /* nasty kludge to fake out the Parse_Item_Text routine */
        current_text = (Linked_Text *) current_text_block;
             /* I have to set this to be anything but null */
        current_item = (Item *) current_text_block;

		place = After_Comma(place);
        current_text_block->x1 = Get_Value(place,default_x);
        place = After_Comma(place);
        current_text_block->y1 = Get_Value(place,default_y);
        place = After_Comma(place);
        current_text_block->pal = Get_Value(place,default_palette);

        current_text_block->next_block = NULL;
        return;
    }

    if (Compare(place,"header ")){
		place = After_Space(place);
        if (Compare(place,"palette")){
            header_palette = Get_Palette(place,header_palette);
            return;
        }
        if (Compare(place,"message")){
            place = After_Equals(place);
            if (*place != '\0'){
                header_message = (char *) malloc(MAX_LINE);
                Transcribe_String(place,header_message);
                return;
            }            
			Critical_Error(VALUE);
        }
        Critical_Error(SYNTAX);
    }

    if (Compare(place,"default ")){
        place = After_Space(place);
        if (Compare(place,"palette")){
            default_palette = Get_Palette(place,default_palette);
            return;
        }
        if (Compare(place,"placement")){
			place = After_Equals(place);
            default_x = Get_Value(place,default_x);
            place = After_Comma(place);
            default_y = Get_Value(place,default_y);
            return;
        }
        Critical_Error(SYNTAX);
    }

    if (Compare(place,"cascade")){
        cascade = YES;
        if (default_x == CENTER){
			default_x = 10;
            default_y = 4;
        }
        cascade_x = Get_Value(place,cascade_x);        
        place = After_Comma(place);
        cascade_y = Get_Value(place,cascade_y);
        return;
    }        

    if (Compare(place,"shadow")){
        draw_shadow = YES;
        shadow_color = Get_Color( place, 8);
		return;
    }        

    if (Compare(place,"backdrop ")){
        place = After_Space(place);
        if (Compare(place,"color")){
            backdrop_color = Get_Color(place, backdrop_color );
            return;
        }
        if (Compare(place,"char")){
            backdrop_char = Get_Value(place,backdrop_char);
            return;
		}
        if (Compare(place,"file")){
            backdrop_file = strdup(After_Equals(place));
            return;
        }
        Critical_Error(SYNTAX);
     }
    if (Compare(place,"notime")){
        show_time=NO;
        return;
	}

	if (Compare(place,"nocomments")){
        allow_comments = NO;
        return;
    }
    if (Compare(place,"comments")){
        allow_comments = YES;
        return;
    }

    if (Compare(place,"noexit")){
        exitable = NO;
        return;
	}

	if (Compare(place,"quickselect")){
		quick_select = YES;
		return;
	}

	if (Compare(place,"noreturn")){
		do_return = NO;
		return;
	}

	Critical_Error(SYNTAX);
}
/***************************************************************************/
void Parse_Menu(t_line)

char t_line[];

{
	char  *place;

	place = After_Comma(++t_line);
	if (*place == '\0')
		l_len--;
	else{
		*(place-1) = '\0';
		l_len = strlen(t_line);
	}

	if (first_menu == NULL){
		first_menu = Allocate_Menu();
		current_menu = first_menu;
	}
	else{
		current_menu->next_menu = Allocate_Menu();
		current_menu = current_menu->next_menu;
	}

	current_menu->first_item      = NULL;         /* set menu defaults */
	current_item                  = NULL;         /*  and other stuff  */
	current_menu->next_menu       = NULL;
	current_menu->number_of_items = 0;
	current_menu->title_length    = l_len;
	strcpy(current_menu->title,t_line);

	current_menu->offset_x = Get_Value(place,default_x);
	place = After_Comma(place);
	current_menu->offset_y = Get_Value(place,default_y);
	place = After_Comma(place);
	current_menu->palette = Get_Palette(place,default_palette);

}

/***************************************************************************/
void Parse_Item(t_line)

char t_line[];

{
	if (first_menu == NULL)
		Critical_Error(POSITION);

	if (current_item == NULL){
		current_item = Allocate_Item();
		current_menu->first_item = current_item;
		current_item->last_item = NULL;
	}
	else{
		current_item->next_item = Allocate_Item();
		current_item->next_item->last_item = current_item;
		current_item = current_item->next_item;
	}

	strcpy(current_item->title,t_line);

	current_item->title_length  = l_len;
	current_text                = NULL;
	current_item->first_line    = NULL;
	current_item->next_item     = NULL;
	current_item->item_number   = ++(current_menu->number_of_items);
}

/***************************************************************************/
void Parse_Item_Text(t_line)

char t_line[];

{
	if (current_item == NULL)
		Critical_Error(POSITION);

	if (current_text == NULL){
		current_item->first_line = Allocate_Linked_Text();
		current_text = current_item->first_line;
	}
	else{
		current_text->next_line = Allocate_Linked_Text();
		current_text = current_text->next_line;
	}

	strcpy(current_text->text_line,++t_line);

	current_text->next_line = NULL;
}

/***************************************************************************/
void Parse_Engine_Exist( )

{
	FILE *Script;
	char temp_line[MAX_LINE+1];

	if ( ( Script = fopen(current_file, "r" ) ) == NULL ){
		strcat(current_file,".mnu");
		if ( ( Script = fopen(current_file, "r" ) ) == NULL ){
			*(current_file+strlen(current_file)-4) = '\0';
//			Critical_Error(FILE_OPEN);
		return;	
		}
	}

	do {
		Read_Line (temp_line, Script );
        if (feof(Script))
            break;
		switch (temp_line[0]){
			case ' ':
				Parse_Item_Text(temp_line);
				break;
			case '%':
				Parse_Menu(temp_line);
				break;
			case '#':
				Parse_Command( temp_line, Script);
			case '\0':                 /* empty line */
			case EOF:
				break;
			default:
				Parse_Item(temp_line);
				break;
		}
	}
    while ( 1 );
	fclose ( Script );
}

/***************************************************************************/
void Parse_Engine( )

{
	FILE *Script;
	char temp_line[MAX_LINE+1];

	if ( ( Script = fopen(current_file, "r" ) ) == NULL ){
		strcat(current_file,".mnu");
		if ( ( Script = fopen(current_file, "r" ) ) == NULL ){
			*(current_file+strlen(current_file)-4) = '\0';
			Critical_Error(FILE_OPEN);
		}
	}

	do {
		Read_Line (temp_line, Script );
        if (feof(Script))
            break;
		switch (temp_line[0]){
			case ' ':
				Parse_Item_Text(temp_line);
				break;
			case '%':
				Parse_Menu(temp_line);
				break;
			case '#':
				Parse_Command( temp_line, Script);
			case '\0':                 /* empty line */
			case EOF:
				break;
			default:
				Parse_Item(temp_line);
				break;
		}
	}
    while ( 1 );
	fclose ( Script );
}

/***************************************************************************/
void Parse( dos_param_2 )

char *dos_param_2;

{
	char main_script[ MAX_FILE_NAME +4 ];
	byte i;

	if (dos_param_2 == NULL)
		Critical_Error(FILE_OPEN);

		/* initialize variables */
	for (i=0;i<=MAX_SPECIAL;i++)
		special_item[i]=NULL;

	current_menu = NULL;
	current_item = NULL;
	current_text = NULL;
	current_text_block = NULL;
	current_file = main_script;
	strcpy (main_script,dos_param_2);

	Parse_Engine();

		/* cover useless special items */
	for (i=0;i<=hot_keys;i++)
		if (special_item[i]->first_line == NULL)
			special_item[i] = NULL;

	current_file = NULL;
		/* tells critical error that we survived parsing */
}
