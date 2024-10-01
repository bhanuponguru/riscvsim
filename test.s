.data
.word 0x12345678
.text
addi x5, x0, -1
lui x6, 16
sh x5, 0(x6)
lh x7, 0(x6)
beq x0, x0, l1
l1: addi x5, x5, 1
beq x5, x0, l1