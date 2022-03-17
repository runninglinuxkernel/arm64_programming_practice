/*
 * pt_regs栈框，用来保存中断现场或者异常现场
 *
 * pt_regs栈框通常位于进程的内核栈的顶部。
 * pt_regs栈框通常位于进程的内核栈的顶部。
 * 而sp的栈顶通常 紧挨着 pt_regs栈框，在pt_regs栈框下方。
 * 保存内容：
 *    x0 ~ x30 通用寄存器
 *    sp
 *    pc
 *    pstate
 */
struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};
