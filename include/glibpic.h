
_geq_
_gueq_  csne _primary, _secondary
	jmp $+4
	mov _primary, #0
	skip
	mov _primary, #1
	ret

_gne_
_gune_  cse _primary, _secondary
	jmp $+4
	mov _primary, #0
	skip
	mov _primary, #1
	ret

_glt_
_gult_  cjb _secondary, _primary, $+7
	mov _primary, #0
	skip
	mov _primary, #1
	ret

_gle_
_gule_  cjbe _secondary, _primary, $+7
	mov _primary, #0
	skip
	mov _primary, #1
	ret

_ggt_
_gugt_  cja _secondary, _primary, $+7
	mov _primary, #0
	skip
	mov _primary, #1
	ret

_gge_
_guge_  cjae _secondary, _primary, $+7
	mov _primary, #0
	skip
	mov _primary, #1
	ret


_bool_  mov W, _primary
	jnz $+4
	mov _primary, #0
	skip
	mov _primary, #1
	ret
