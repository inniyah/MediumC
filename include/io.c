/* set IO port mode */

SetP_A( value )
char value;
{
#asm
	mov _primary, #0
	add _primary, _stackptr
	call _indr_
	mov !ra , _primary
#endasm
}


SetP_B( value )
char value;
{
#asm
	mov _primary, #0
	add _primary, _stackptr
	call _indr_
	mov !rb , _primary
	ret
#endasm
}

/* read IO port value */

RdPortA()
{
#asm
	mov _primary, _portA
#endasm
}


RdPortB ()
{
#asm
	mov _primary, _portB
#endasm
}

/* write IO port value */

WrPortA( value )
char value;
{
#asm
	mov _primary, #0
	add _primary, _stackptr
	call _indr_
	mov _portA, _primary
#endasm
}


WrPortB( value )
char value;
{
#asm
	mov _primary, #0
	add _primary, _stackptr
	call _indr_
	mov _portB, _primary
#endasm
}


