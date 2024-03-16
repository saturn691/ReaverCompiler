	.file	"modulo.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_f2p0_d2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	f
	.type	f, @function
f:
	addi	sp,sp,-16
	sw	s0,12(sp)
	addi	s0,sp,16
	nop
	fmv.w.x	fa0,a5
	lw	s0,12(sp)
	addi	sp,sp,16
	jr	ra
	.size	f, .-f
	.ident	"GCC: () 12.2.0"
