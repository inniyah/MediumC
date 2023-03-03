
_asr_ clc
      mov W,>>_secondary
      mov _secondary, W
      djnz _primary, $-3
      mov _primary,_secondary
      ret

_asl_ clc
      mov W,<<_secondary
      mov _secondary, W
      djnz _primary, $-3
      mov _primary,_secondary
      ret

_sub_ mov _temp, _secondary
      sub _temp, _primary
      mov _primary, _temp
      ret

