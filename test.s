.data
.dword 0x0123456789abcdef
.text
lui x3, 0x10
ld s0, 0(x3)
