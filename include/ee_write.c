
ee_write( addr, data )
char addr;
char data;
{
#asm
        clrb _fr3.5           ; set page 0
        clrb _fr3.6

	mov _primary, #1
	add _primary, _stackptr
	mov _fr4, _primary
	mov _primary, _fr0
	mov _eeaddr, _primary

	mov _primary, #0
	add _primary, _stackptr
	mov _fr4, _primary
	mov _primary, _fr0
	mov _eedata, _primary

        setb _fr3.5           ; set page 1
        clrb _fr3.6


        clrb _eecon1.4        ; clr eeif flag
        setb _eecon1.2        ; enable write


        mov W, #55h           ; magic sequence
        mov _eecon2, W
        mov W, #0aah
        mov _eecon2, W
        setb _eecon1.1        ; write command

        jnb _eecon1.4, $      ; loop until done

        clrb _eecon1.4        ; clr eeif flag

        clrb _fr3.5           ; set page 0
        clrb _fr3.6

#endasm
}
