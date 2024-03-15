	.file	"103-for_continue.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_f2p0_d2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
.text
	.align	2
	.globl	f
	.type	f, @function
f:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	zero,-20(s0)
	sw	zero,-24(s0)
	j	.L2
.L5:				// statement
	lw	a4,-24(s0) // i
	lw	a5,-36(s0) // x
	bge	a4,a5,.L7  // if (i >= x) continue;
	lw	a5,-20(s0) // i
	addi	a5,a5,1 // i += 1
	sw	a5,-20(s0) // store i
	j	.L4
.L7:
	nop
.L4:
	lw	a5,-24(s0)	// counter
	addi	a5,a5,1	// counter += 1
	sw	a5,-24(s0)	// store counter
.L2:				// condition
	lw	a4,-24(s0)
	li	a5,9
	ble	a4,a5,.L5
	lw	a5,-20(s0)
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	f, .-f
	.ident	"GCC: () 12.2.0"
