	.file	"scoped_var.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_f2p0_d2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	f
	.type	f, @function
f:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	li	a5,4096
	addi	a5,a5,1582
	sw	a5,-20(s0)
	li	a5,1234
	sw	a5,-24(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	f, .-f
	.ident	"GCC: () 12.2.0"
