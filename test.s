 .text
 lui x3, 0x10
 addi t2, t2, 4
sd t2, 0(x3)
ld t0, 0(x3)
ld t1, 8(x3)
ld t2, 16(x3)
ld t3, 24(x3)
ld t4, 32(x3)
ld t5, 40(x3)
