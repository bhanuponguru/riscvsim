 .text
 lui x3, 0x10
 lui t6, 0x11
 addi t2, t2, 4
sd t2, 0(x3)
ld t0, 0(x3)
ld t1, 0(t6)
ld t1, 8(t6)
sd t2, 16(x3)
ld t2, 16(x3)
ld t3, 16(t6)
ld t3, 24(x3)
ld t3, 24(t6)
addi t2, t2, 4
sd t2, 32(x3)
ld t4, 32(x3)
ld t5, 40(x3)
