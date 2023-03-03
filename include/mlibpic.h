;
;  pic arithmetic library routines
;
;  this is really quick and dirty (stupid) coding
;

;  multiply primary register by secondary register

_mul_
       mov W, _primary
       jz _mul3_
       mov W, _secondary
       jz _mul4_

; repeated adds - yuck!

       mov _temp, #0
_mul1_ add _temp, _primary
       djnz _secondary, _mul1_
       mov _primary, _temp
       ret

_mul4_ mov _primary,#0
_mul3_ ret

;
;      division of secondary register by primary register
;      aaaack! repeated subtractions

_div_
       mov W, _primary     ; a division by zero!
       jz _div3_
       mov W, _secondary
       jz _div4_

       mov _temp, _secondary
       mov _secondary, #0

_div1_ sub _temp, _primary
       jnc _div2_
       inc _secondary
       jmp _div1_

_div2_ add _temp, _primary
       mov _primary, _secondary
       mov _secondary, _temp
       ret

_div3_ mov _primary, #255   ; divide by zero, set reg to 255
       mov _secondary, #0
       ret

_div4_ mov _secondary, _primary
       mov _primary,#0
       ret
