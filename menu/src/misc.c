/***************************************************************************/
/* Memory allocation Routines */

Menu *Allocate_Menu()

{
	Menu *temp_menu;
	if ((temp_menu = (Menu *) malloc(sizeof(Menu)+l_len)) == NULL)
		Critical_Error(MEMORY);
	return(temp_menu);
}

Item *Allocate_Item()
{
	Item *temp_item;
	if ((temp_item = (Item *) malloc (sizeof(Item)+l_len+1)) == NULL)
		Critical_Error(MEMORY);
	return (temp_item);
}

Linked_Text *Allocate_Linked_Text()
{
	Linked_Text *temp_text;
	if ((temp_text = (Linked_Text *) malloc (sizeof(Linked_Text)+l_len)) == NULL)
		Critical_Error(MEMORY);
	return (temp_text);
}

/***************************************************************************/
boolean Compare(string1,string2)

char *string1,*string2;

{
	while (*string2 != '\0')
		if( (*string1++ | 32) != (*string2++ | 32) )
			return(NO);
	return(YES);
}

/***************************************************************************/
char *After_Space(place)

char *place;

{
	while ( *place != ' ' )
		 if ( *++place == '\0' )
			return(place);
	place++;
	while ( *place == ' ')
		place++;
	return(place);
}

/***************************************************************************/
char *After_Equals(place)

char *place;

{
	while ( *place != '=' )
		 if ( *++place == '\0' )
			return(place);
	place++;
	while ( *place == ' ')
		place++;

	return(place);
}

/***************************************************************************/
char *After_Comma(place)

char *place;

{
	while ( *place != '\0' && *place != ',') place++;
	if (*place == ',') place++;
	while ( *place == ' ') place++;
	return(place);
}

/***************************************************************************/
byte Get_Value(place,def_value)

char *place;
byte def_value;

{
	int value;

	while ( (*place < '0') || (*place > '9'))
		if (*place == '\0' || *(place++) == ',')
			return(def_value);

	while ( (*place >= '0') && (*place <= '9'))
		place++;

    value = *(--place) - 48;
    if (*(--place) >= '0' && *place <= '9'){
        value += ( (*place - 48) * 10 );
        if (*(--place) >= '0' && *place <= '9')
            value += ( (*place - 48) * 100 );
    }    

    if (value >255)
        Critical_Error(VALUE);

    return ( (byte) value );
}

/***************************************************************************/
byte Get_Palette(place,def_value)

char *place;
byte def_value;

{
    byte value;

    if (!color_monitor)
        return(def_value);
    value = Get_Value(place,def_value);
    if (value >= MAX_PALETTES)
        return(def_value);
    return(value);
}
/***************************************************************************/
byte Get_Color(place,def_value)

char *place;
byte def_value;

{
    byte value;

    if (!color_monitor) return(def_value);
    value = (byte) ( Get_Value( place,( byte)(def_value >>4) ) <<4 );
    place = After_Comma(place);
    value = value | Get_Value(place,(byte)(def_value & 15) );
    return(value);
}




/*************************************************************************/
int far Crit_Error_Handler( unsigned deverror, unsigned errcode, unsigned far *devhdr ) {
    return(_HARDERR_FAIL);
}

// _getdiskfree() or _dos_getdiskfree() may be able to replace Disk_Space_Check.
/**************************************************************************/
long int Disk_Space_Check( drive )

byte drive;

{
	typedef void far interrupt (*Int_Vector)( void );
	Int_Vector old_vector;
	int a=0,b=0,c=0;

    // Interupt 24h is generated by DOS if a hardware error occurs, _harderr()
    // installs a handler which will be called if interrupt 24h occurs.
    old_vector = _dos_getvect(0x24);
    _harderr( Crit_Error_Handler );

	// find disk space
 	_asm mov  ah,36h
 	_asm mov  dl,drive
 	_asm int  21h
 	_asm mov  a,ax
 	_asm mov  b,bx
 	_asm mov  c,cx

    _dos_setvect(0x24, old_vector);

	/* check for success */
	if ( a == -1 )
		return( -1 );
		return( (long)(a*(long)(b*c)) );
}

/***************************************************************************/
void Critical_Error(error)

error_type error;

{
	 Clean_Up();
	 if (current_file != NULL){
		  fprintf(stderr,"file- %s\n",current_file);
		  if (line_count)
				fprintf(stderr,"line- %u\n",line_count);
	 }
	 switch(error){
			case SYNTAX:
				fputs("Syntax Error.",stderr);
				break;
			case VALUE:
				fputs("Illegal or Missing Value.",stderr);
				break;
		  case POSITION:
				fputs("Unexpected menu structure.",stderr);
				break;
		  case MEMORY:
				fputs("Memory allocation error.",stderr);
				break;
		  case FILE_OPEN:
				fputs("Unable to open file.",stderr);
				break;
		  case DISPLAY:
				fputs("Unable to display menu script.",stderr);
				break;
		  case ENVIROMENT:
				fputs("Improper 't' enviroment variable",stderr);
				break;
		  case PARAM:
				fputs("Bad Command Line Parameter",stderr);
				break;
	 }
	fputc('\n',stderr);
	 exit(LEAVE_MENU);
}

/**************************************************************************/
boolean Get_DM_Var( place, temp, space )

char *place, *temp;
byte space;

{
    byte i = 0;
    
    for (i=0;i<var_number;i++){
        if (Compare(place,vars[i].name)){
			strncpy(temp,vars[i].value,space);
            return(YES);
        }
    }
    return(NO);
}

/**************************************************************************/
void Transcribe_String(old,new)

char *old,*new;

{
    byte space = MAX_LINE;

	*new = '\0';

    for (;;){
            /* check for variables */
        while (*old == '%'){
            if ( !Get_DM_Var(old,new,space)){
                Find_Env_Var(old,new,space);
                if (*new == VAR_NOT_FOUND){
					break;    /* leave the while loop if it's not a variable */
                }
            }

      /* move past variable */
            while (*new != '\0'){
                space--;
				new++;
            }

            old++;
            while (*old != '%'){
                old++;
            }
            old++;

            if (!space)  /* stop if no room left in allocated space */
                return;
        }

        if (*old == '\0' || !(--space) ){
            *new = '\0';
			return;
        }
        *new = *old;
        new++;
        old++;
    }
}

/***************************************************************************/
boolean Fcompare( string1,string2 )

char *string1, far *string2;

{
        /* skip first '%' */
	string1++;

    while (*string2 != '='){
        if( (*string1++ | 32) != (*string2++ | 32) )
            return(NO);
    }
        /* does it have a closing '%' ? */
    if (*string1 == '%')
		return(YES);
    return(NO);
}

/***************************************************************************/
void Find_Env_Var( var, string, space )

char *var, *string;
byte space;

{
    char far *cptr = env_start;

    while (!Fcompare(var,cptr)){
        while (*cptr != '\0')
			cptr++;
        if (*(++cptr) == '\0'){
            *string = VAR_NOT_FOUND;
            return;
        }
    }
    while (*cptr++ != '=');
    
    while (*cptr != '\0'){
        *string++ = *cptr++;
        if (--space == 0)
            break;
    }
    *string  = '\0';
    return;
}