/* get a character from string str, offset index */
/* ie, str[index] */

GetChar( str, index )
char str;
char index;
{
#asm
	sub _stackptr, #1
	mov _primary, #0
	add _primary, _stackptr
	call _push_
	mov _primary, #3
	add _primary, _stackptr
	call _indr_
	call _push_
	mov _primary, #3
	add _primary, _stackptr
	call _indr_
	call _pop_
	add _primary, _secondary
	call _pop_
	call _putstk_
	mov _primary, #0
	add _primary, _stackptr
	call _indr_
	mov W, _primary
	call _string_table_
	mov _primary, W
	add _stackptr, #1
#endasm
}

