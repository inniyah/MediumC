;
; setup for pic 16c84
; can be modified for individual projects
;
; equates
;

_CSEG = 40h         ; code origin
_DSEG = 0fh         ; start of working data registers

; registers needed by code generator

_primary = 2fh
_secondary = 2eh
_temp = 2ch
_stackptr = 2bh

; some standard pic function registers
_fr0 = 0h
_fr1 = 1h
_fr2 = 2h
_fr3 = 3h
_fr4 = 4h
_portA = 5h
_portB = 6h

_eedata = 08h
_eeaddr = 09h
_eecon1 = 08h
_eecon2 = 09h

	device PIC16C84

	org 0

	jmp startup

	org 4

	jmp interrupt

	org 10

startup
	mov _stackptr, #_stackptr		;set up stack pointer
	call main_
	jmp $												;infinite loop if return from main

interrupt
	jmp $												;set to your interrupt routine 

; standard stack manipulation code

_push_
	dec _stackptr
	mov _fr4,	_stackptr
	mov _fr0, _primary
	ret

_pop_
	mov	_fr4,	_stackptr
	mov _secondary, _fr0
	inc _stackptr
	ret

_putstk_
	mov _fr4, _secondary
	mov _fr0, _primary
	ret

_indr_
	mov _fr4, _primary
	mov _primary, _fr0
	ret

_swap_
	mov _temp, _primary
	mov _primary, _secondary
	mov _secondary, _temp
	ret

_swaps_
	mov _fr4, _stackptr
	mov _temp, _fr0
	mov _fr0, _primary
	mov _primary, _temp
	ret

;***********************Start of user code**********************
