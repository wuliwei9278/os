	.globl	TaskSwitch
TaskSwitch:					
	stmia	r0, {r0-r14}
	ldmia	r1, {r0-r14}
	bx	lr