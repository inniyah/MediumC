
ee_read( addr )
char addr;
{
#asm

	mov _primary, #0        ; get argument
	add _primary, _stackptr
	mov _fr4, _primary
	mov _primary, _fr0
	mov _eeaddr, _primary
        setb _fr3.5             ; set page 1
        clrb _fr3.6
        setb _eecon1.0          ; initialize read
        clrb _fr3.5             ; set page 0
        clrb _fr3.6
        mov _primary, _eedata
#endasm
}


