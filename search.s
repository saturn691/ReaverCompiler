	.file	"search.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_f2p0_d2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	search
	.type	search, @function
search:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
	mv	a5,a1
	sb	a5,-21(s0)
	j	.L2
.L5:
	lw	a5,-20(s0)
	lbu	a5,0(a5)
	lbu	a4,-21(s0)
	bne	a4,a5,.L3
	lw	a5,-20(s0)
	j	.L4
.L3:
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lw	a5,-20(s0)
	lbu	a5,0(a5)
	bne	a5,zero,.L5
	li	a5,0
.L4:
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	search, .-search
	.ident	"GCC: () 12.2.0"
