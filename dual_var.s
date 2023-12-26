	.file	"dual_var.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.globl	f
	.type	f, @function
f:
	addi	sp,sp,-32
	sd	s0,24(sp)
	addi	s0,sp,32
	li	a5,1234
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	mv	a0,a5
	ld	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	f, .-f
	.ident	"GCC: () 12.2.0"
