/* $Id: fpu.c,v 1.4 2004/01/13 05:52:11 kkojima Exp $
 *
 * linux/arch/sh/kernel/fpu.c
 *
 * Save/restore floating point context for signal handlers.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1999, 2000  Kaz Kojima & Niibe Yutaka
 * Copyright (C) 2006  ST Microelectronics Ltd. (denorm support)
 *
 * FIXME! These routines have not been tested for big endian case.
 */

#include <linux/sched.h>
#include <linux/signal.h>
#include <asm/processor.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/fpu.h>
#include "sh4_fpu.h"

/* The PR (precision) bit in the FP Status Register must be clear when
 * an frchg instruction is executed, otherwise the instruction is undefined.
 * Executing frchg with PR set causes a trap on some SH4 implementations.
 */

#define FPSCR_RCHG 0x00000000
extern unsigned long long float64_div(unsigned long long a,
				      unsigned long long b);
extern unsigned long int float32_div(unsigned long int a, unsigned long int b);
extern unsigned long long float64_mul(unsigned long long a,
				      unsigned long long b);
extern unsigned long int float32_mul(unsigned long int a, unsigned long int b);
extern unsigned long long float64_add(unsigned long long a,
				      unsigned long long b);
extern unsigned long int float32_add(unsigned long int a, unsigned long int b);
extern unsigned long long float64_sub(unsigned long long a,
				      unsigned long long b);
extern unsigned long int float32_sub(unsigned long int a, unsigned long int b);
extern unsigned long int float64_to_float32(unsigned long long a);
static unsigned int fpu_exception_flags;

/*
 * Save FPU registers onto task structure.
 */
void save_fpu(struct task_struct *tsk)
{
	unsigned long dummy;

	enable_fpu();
	asm volatile ("sts.l	fpul, @-%0\n\t"
		      "sts.l	fpscr, @-%0\n\t"
		      "lds	%2, fpscr\n\t"
		      "frchg\n\t"
		      "fmov.s	fr15, @-%0\n\t"
		      "fmov.s	fr14, @-%0\n\t"
		      "fmov.s	fr13, @-%0\n\t"
		      "fmov.s	fr12, @-%0\n\t"
		      "fmov.s	fr11, @-%0\n\t"
		      "fmov.s	fr10, @-%0\n\t"
		      "fmov.s	fr9, @-%0\n\t"
		      "fmov.s	fr8, @-%0\n\t"
		      "fmov.s	fr7, @-%0\n\t"
		      "fmov.s	fr6, @-%0\n\t"
		      "fmov.s	fr5, @-%0\n\t"
		      "fmov.s	fr4, @-%0\n\t"
		      "fmov.s	fr3, @-%0\n\t"
		      "fmov.s	fr2, @-%0\n\t"
		      "fmov.s	fr1, @-%0\n\t"
		      "fmov.s	fr0, @-%0\n\t"
		      "frchg\n\t"
		      "fmov.s	fr15, @-%0\n\t"
		      "fmov.s	fr14, @-%0\n\t"
		      "fmov.s	fr13, @-%0\n\t"
		      "fmov.s	fr12, @-%0\n\t"
		      "fmov.s	fr11, @-%0\n\t"
		      "fmov.s	fr10, @-%0\n\t"
		      "fmov.s	fr9, @-%0\n\t"
		      "fmov.s	fr8, @-%0\n\t"
		      "fmov.s	fr7, @-%0\n\t"
		      "fmov.s	fr6, @-%0\n\t"
		      "fmov.s	fr5, @-%0\n\t"
		      "fmov.s	fr4, @-%0\n\t"
		      "fmov.s	fr3, @-%0\n\t"
		      "fmov.s	fr2, @-%0\n\t"
		      "fmov.s	fr1, @-%0\n\t"
		      "fmov.s	fr0, @-%0\n\t"
		      "lds	%3, fpscr\n\t":"=r" (dummy)
		      :"0"((char *)(&tsk->thread.fpu.hard.status)),
		      "r"(FPSCR_RCHG), "r"(FPSCR_INIT)
		      :"memory");

	disable_fpu();
}

static void restore_fpu(struct task_struct *tsk)
{
	unsigned long dummy;

	enable_fpu();
	asm volatile ("lds	%2, fpscr\n\t"
		      "fmov.s	@%0+, fr0\n\t"
		      "fmov.s	@%0+, fr1\n\t"
		      "fmov.s	@%0+, fr2\n\t"
		      "fmov.s	@%0+, fr3\n\t"
		      "fmov.s	@%0+, fr4\n\t"
		      "fmov.s	@%0+, fr5\n\t"
		      "fmov.s	@%0+, fr6\n\t"
		      "fmov.s	@%0+, fr7\n\t"
		      "fmov.s	@%0+, fr8\n\t"
		      "fmov.s	@%0+, fr9\n\t"
		      "fmov.s	@%0+, fr10\n\t"
		      "fmov.s	@%0+, fr11\n\t"
		      "fmov.s	@%0+, fr12\n\t"
		      "fmov.s	@%0+, fr13\n\t"
		      "fmov.s	@%0+, fr14\n\t"
		      "fmov.s	@%0+, fr15\n\t"
		      "frchg\n\t"
		      "fmov.s	@%0+, fr0\n\t"
		      "fmov.s	@%0+, fr1\n\t"
		      "fmov.s	@%0+, fr2\n\t"
		      "fmov.s	@%0+, fr3\n\t"
		      "fmov.s	@%0+, fr4\n\t"
		      "fmov.s	@%0+, fr5\n\t"
		      "fmov.s	@%0+, fr6\n\t"
		      "fmov.s	@%0+, fr7\n\t"
		      "fmov.s	@%0+, fr8\n\t"
		      "fmov.s	@%0+, fr9\n\t"
		      "fmov.s	@%0+, fr10\n\t"
		      "fmov.s	@%0+, fr11\n\t"
		      "fmov.s	@%0+, fr12\n\t"
		      "fmov.s	@%0+, fr13\n\t"
		      "fmov.s	@%0+, fr14\n\t"
		      "fmov.s	@%0+, fr15\n\t"
		      "frchg\n\t"
		      "lds.l	@%0+, fpscr\n\t"
		      "lds.l	@%0+, fpul\n\t"
		      :"=r" (dummy)
		      :"0"(&tsk->thread.fpu), "r"(FPSCR_RCHG)
		      :"memory");
	disable_fpu();
}

/*
 * Load the FPU with signalling NANS.  This bit pattern we're using
 * has the property that no matter wether considered as single or as
 * double precision represents signaling NANS.  
 */

static void fpu_init(void)
{
	enable_fpu();
	asm volatile (	"lds	%0, fpul\n\t"
			"lds	%1, fpscr\n\t"
			"fsts	fpul, fr0\n\t"
			"fsts	fpul, fr1\n\t"
			"fsts	fpul, fr2\n\t"
			"fsts	fpul, fr3\n\t"
			"fsts	fpul, fr4\n\t"
			"fsts	fpul, fr5\n\t"
			"fsts	fpul, fr6\n\t"
			"fsts	fpul, fr7\n\t"
			"fsts	fpul, fr8\n\t"
			"fsts	fpul, fr9\n\t"
			"fsts	fpul, fr10\n\t"
			"fsts	fpul, fr11\n\t"
			"fsts	fpul, fr12\n\t"
			"fsts	fpul, fr13\n\t"
			"fsts	fpul, fr14\n\t"
			"fsts	fpul, fr15\n\t"
			"frchg\n\t"
			"fsts	fpul, fr0\n\t"
			"fsts	fpul, fr1\n\t"
			"fsts	fpul, fr2\n\t"
			"fsts	fpul, fr3\n\t"
			"fsts	fpul, fr4\n\t"
			"fsts	fpul, fr5\n\t"
			"fsts	fpul, fr6\n\t"
			"fsts	fpul, fr7\n\t"
			"fsts	fpul, fr8\n\t"
			"fsts	fpul, fr9\n\t"
			"fsts	fpul, fr10\n\t"
			"fsts	fpul, fr11\n\t"
			"fsts	fpul, fr12\n\t"
			"fsts	fpul, fr13\n\t"
			"fsts	fpul, fr14\n\t"
			"fsts	fpul, fr15\n\t"
			"frchg\n\t"
			"lds	%2, fpscr\n\t"
			:	/* no output */
		      	:"r" (0), "r"(FPSCR_RCHG), "r"(FPSCR_INIT));
	disable_fpu();
}

/**
 *      denormal_to_double - Given denormalized float number,
 *                           store double float
 *
 *      @fpu: Pointer to sh_fpu_hard structure
 *      @n: Index to FP register
 */
static void denormal_to_double(struct sh_fpu_hard_struct *fpu, int n)
{
	unsigned long du, dl;
	unsigned long x = fpu->fpul;
	int exp = 1023 - 126;

	if (x != 0 && (x & 0x7f800000) == 0) {
		du = (x & 0x80000000);
		while ((x & 0x00800000) == 0) {
			x <<= 1;
			exp--;
		}
		x &= 0x007fffff;
		du |= (exp << 20) | (x >> 3);
		dl = x << 29;

		fpu->fp_regs[n] = du;
		fpu->fp_regs[n + 1] = dl;
	}
}

/**
 *	ieee_fpe_handler - Handle denormalized number exception
 *
 *	@regs: Pointer to register structure
 *
 *	Returns 1 when it's handled (should not cause exception).
 */
static int ieee_fpe_handler(struct pt_regs *regs)
{
	unsigned short insn = *(unsigned short *)regs->pc;
	unsigned short finsn;
	unsigned long nextpc;
	int nib[4] = {
		(insn >> 12) & 0xf,
		(insn >> 8) & 0xf,
		(insn >> 4) & 0xf,
		insn & 0xf
	};

	if (nib[0] == 0xb || (nib[0] == 0x4 && nib[2] == 0x0 && nib[3] == 0xb))
		regs->pr = regs->pc + 4;  /* bsr & jsr */

	if (nib[0] == 0xa || nib[0] == 0xb) {
		/* bra & bsr */
		nextpc = regs->pc + 4 + ((short)((insn & 0xfff) << 4) >> 3);
		finsn = *(unsigned short *)(regs->pc + 2);
	} else if (nib[0] == 0x8 && nib[1] == 0xd) {
		/* bt/s */
		if (regs->sr & 1)
			nextpc = regs->pc + 4 + ((char)(insn & 0xff) << 1);
		else
			nextpc = regs->pc + 4;
		finsn = *(unsigned short *)(regs->pc + 2);
	} else if (nib[0] == 0x8 && nib[1] == 0xf) {
		/* bf/s */
		if (regs->sr & 1)
			nextpc = regs->pc + 4;
		else
			nextpc = regs->pc + 4 + ((char)(insn & 0xff) << 1);
		finsn = *(unsigned short *)(regs->pc + 2);
	} else if (nib[0] == 0x4 && nib[3] == 0xb && (nib[2] == 0x0 || nib[2] == 0x2)) {
		/* jmp & jsr */
		nextpc = regs->regs[nib[1]];
		finsn = *(unsigned short *)(regs->pc + 2);
	} else if (nib[0] == 0x0 && nib[3] == 0x3 && (nib[2] == 0x0 || nib[2] == 0x2)) {
		/* braf & bsrf */
		nextpc = regs->pc + 4 + regs->regs[nib[1]];
		finsn = *(unsigned short *)(regs->pc + 2);
	} else if (insn == 0x000b) {
		/* rts */
		nextpc = regs->pr;
		finsn = *(unsigned short *)(regs->pc + 2);
	} else {
		nextpc = regs->pc + instruction_size(insn);
		finsn = insn;
	}

	if ((finsn & 0xf1ff) == 0xf0ad) {
		/* fcnvsd */
		struct task_struct *tsk = current;

		if ((tsk->thread.fpu.hard.fpscr & FPSCR_CAUSE_ERROR)) {
			/* FPU error */
			denormal_to_double(&tsk->thread.fpu.hard,
					   (finsn >> 8) & 0xf);
		} else
			return 0;

		regs->pc = nextpc;
		return 1;
	} else if ((finsn & 0xf00f) == 0xf002) {
		/* fmul */
		struct task_struct *tsk = current;
		int fpscr;
		int n, m, prec;
		unsigned int hx, hy;

		n = (finsn >> 8) & 0xf;
		m = (finsn >> 4) & 0xf;
		hx = tsk->thread.fpu.hard.fp_regs[n];
		hy = tsk->thread.fpu.hard.fp_regs[m];
		fpscr = tsk->thread.fpu.hard.fpscr;
		prec = fpscr & FPSCR_DBL_PRECISION;

		if ((fpscr & FPSCR_CAUSE_ERROR)
		    && (prec && ((hx & 0x7fffffff) < 0x00100000
				 || (hy & 0x7fffffff) < 0x00100000))) {
			long long llx, lly;

			/* FPU error because of denormal (doubles) */
			llx = ((long long)hx << 32)
			    | tsk->thread.fpu.hard.fp_regs[n + 1];
			lly = ((long long)hy << 32)
			    | tsk->thread.fpu.hard.fp_regs[m + 1];
			llx = float64_mul(llx, lly);
			tsk->thread.fpu.hard.fp_regs[n] = llx >> 32;
			tsk->thread.fpu.hard.fp_regs[n + 1] = llx & 0xffffffff;
		} else if ((fpscr & FPSCR_CAUSE_ERROR)
			   && (!prec && ((hx & 0x7fffffff) < 0x00800000
					 || (hy & 0x7fffffff) < 0x00800000))) {
			/* FPU error because of denormal (floats) */
			hx = float32_mul(hx, hy);
			tsk->thread.fpu.hard.fp_regs[n] = hx;
		} else
			return 0;

		regs->pc = nextpc;
		return 1;
	} else if ((finsn & 0xf00e) == 0xf000) {
		/* fadd, fsub */
		struct task_struct *tsk = current;
		int fpscr;
		int n, m, prec;
		unsigned int hx, hy;

		n = (finsn >> 8) & 0xf;
		m = (finsn >> 4) & 0xf;
		hx = tsk->thread.fpu.hard.fp_regs[n];
		hy = tsk->thread.fpu.hard.fp_regs[m];
		fpscr = tsk->thread.fpu.hard.fpscr;
		prec = fpscr & FPSCR_DBL_PRECISION;

		if ((fpscr & FPSCR_CAUSE_ERROR)
		    && (prec && ((hx & 0x7fffffff) < 0x00100000
				 || (hy & 0x7fffffff) < 0x00100000))) {
			long long llx, lly;

			/* FPU error because of denormal (doubles) */
			llx = ((long long)hx << 32)
			    | tsk->thread.fpu.hard.fp_regs[n + 1];
			lly = ((long long)hy << 32)
			    | tsk->thread.fpu.hard.fp_regs[m + 1];
			if ((finsn & 0xf00f) == 0xf000)
				llx = float64_add(llx, lly);
			else
				llx = float64_sub(llx, lly);
			tsk->thread.fpu.hard.fp_regs[n] = llx >> 32;
			tsk->thread.fpu.hard.fp_regs[n + 1] = llx & 0xffffffff;
		} else if ((fpscr & FPSCR_CAUSE_ERROR)
			   && (!prec && ((hx & 0x7fffffff) < 0x00800000
					 || (hy & 0x7fffffff) < 0x00800000))) {
			/* FPU error because of denormal (floats) */
			if ((finsn & 0xf00f) == 0xf000)
				hx = float32_add(hx, hy);
			else
				hx = float32_sub(hx, hy);
			tsk->thread.fpu.hard.fp_regs[n] = hx;
		} else
			return 0;

		regs->pc = nextpc;
		return 1;

	} else if ((finsn & 0xf003) == 0xf003) {
		/* fdiv */
		struct task_struct *tsk = current;
		int fpscr;
		int n, m, prec;
		unsigned int hx, hy;

		n = (finsn >> 8) & 0xf;
		m = (finsn >> 4) & 0xf;
		hx = tsk->thread.fpu.hard.fp_regs[n];
		hy = tsk->thread.fpu.hard.fp_regs[m];
		fpscr = tsk->thread.fpu.hard.fpscr;
		prec = fpscr & FPSCR_DBL_PRECISION;

		if ((fpscr & FPSCR_CAUSE_ERROR)
		    && (prec && ((hx & 0x7fffffff) < 0x00100000
				 || (hy & 0x7fffffff) < 0x00100000))) {
			long long llx, lly;

			/* FPU error because of denormal (doubles) */
			llx = ((long long)hx << 32)
			    | tsk->thread.fpu.hard.fp_regs[n + 1];
			lly = ((long long)hy << 32)
			    | tsk->thread.fpu.hard.fp_regs[m + 1];

			llx = float64_div(llx, lly);

			tsk->thread.fpu.hard.fp_regs[n] = llx >> 32;
			tsk->thread.fpu.hard.fp_regs[n + 1] = llx & 0xffffffff;
		} else if ((fpscr & FPSCR_CAUSE_ERROR)
			   && (!prec && ((hx & 0x7fffffff) < 0x00800000
					 || (hy & 0x7fffffff) < 0x00800000))) {
			/* FPU error because of denormal (floats) */
			hx = float32_div(hx, hy);
			tsk->thread.fpu.hard.fp_regs[n] = hx;
		} else
			return 0;

		regs->pc = nextpc;
		return 1;

	} else if ((finsn & 0xf0bd) == 0xf0bd) {
		/* fcnvds - double to single precision convert */
		struct task_struct *tsk = current;
		int m;
		unsigned int hx;

		m = (finsn >> 8) & 0x7;
		hx = tsk->thread.fpu.hard.fp_regs[m];

		if ((tsk->thread.fpu.hard.fpscr & FPSCR_CAUSE_ERROR)
			&& ((hx & 0x7fffffff) < 0x00100000)) {
			/* subnormal double to float conversion */
			long long llx;

			llx = ((long long)tsk->thread.fpu.hard.fp_regs[m] << 32)
			    | tsk->thread.fpu.hard.fp_regs[m + 1];

			tsk->thread.fpu.hard.fpul = float64_to_float32(llx);
		} else
			return 0;

		regs->pc = nextpc;
		return 1;
	} else
		return 0;
}

void float_raise(unsigned int flags)
{
	fpu_exception_flags |= flags;
}

int float_rounding_mode(void)
{
	struct task_struct *tsk = current;
	int roundingMode = FPSCR_ROUNDING_MODE(tsk->thread.fpu.hard.fpscr);
	return roundingMode;
}

asmlinkage void
do_fpu_error(unsigned long r4, unsigned long r5, unsigned long r6,
	     unsigned long r7, struct pt_regs __regs)
{
	struct pt_regs *regs = RELOC_HIDE(&__regs, 0);
	struct task_struct *tsk = current;

	__unlazy_fpu(tsk, regs);
	fpu_exception_flags = 0;
	if (ieee_fpe_handler(regs)) {
		tsk->thread.fpu.hard.fpscr &=
		    ~(FPSCR_CAUSE_MASK | FPSCR_FLAG_MASK);
		tsk->thread.fpu.hard.fpscr |= fpu_exception_flags;
		/* Set the FPSCR flag as well as cause bits - simply replicate the cause */
		tsk->thread.fpu.hard.fpscr |= (fpu_exception_flags >> 10);
		grab_fpu(regs);
		restore_fpu(tsk);
		task_thread_info(tsk)->status |= TS_USEDFPU;
		if ((((tsk->thread.fpu.hard.fpscr & FPSCR_ENABLE_MASK) >> 7) &
		     (fpu_exception_flags >> 2)) == 0) {
			return;
		}
	}

	force_sig(SIGFPE, tsk);
}

void fpu_state_restore(struct pt_regs *regs)
{
	struct task_struct *tsk = current;

	grab_fpu(regs);
	if (unlikely(!user_mode(regs))) {
		BUG();
		return;
	}

	if (likely(used_math())) {
		/* Using the FPU again.  */
		restore_fpu(tsk);
	} else {
		/* First time FPU user.  */
		fpu_init();
		set_used_math();
	}
	task_thread_info(tsk)->status |= TS_USEDFPU;
	tsk->fpu_counter++;
}

asmlinkage void
do_fpu_state_restore(unsigned long r4, unsigned long r5, unsigned long r6,
		     unsigned long r7, struct pt_regs __regs)
{
	struct pt_regs *regs = RELOC_HIDE(&__regs, 0);

	fpu_state_restore(regs);
}
