/*
 * File      : trap.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2015, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-01-13     weety      modified from mini2440
 * 2015-04-15     ArdaFu     Split from AT91SAM9260 BSP
 */

#include <rtthread.h>
#include <rthw.h>

#define INT_IRQ     0x00
#define INT_FIQ     0x01

extern struct rt_thread *rt_current_thread;
#ifdef RT_USING_FINSH
extern long list_thread(void);
#endif

struct rt_hw_register
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t fp;
    rt_uint32_t ip;
    rt_uint32_t sp;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t spsr;
    rt_uint32_t cpsr;
};

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */

void rt_hw_show_register (struct rt_hw_register *regs)
{
    rt_kprintf("Execption:\n");
    rt_kprintf("r00:0x%08x r01:0x%08x r02:0x%08x r03:0x%08x\n",
               regs->r0, regs->r1, regs->r2, regs->r3);
    rt_kprintf("r04:0x%08x r05:0x%08x r06:0x%08x r07:0x%08x\n",
               regs->r4, regs->r5, regs->r6, regs->r7);
    rt_kprintf("r08:0x%08x r09:0x%08x r10:0x%08x\n",
               regs->r8, regs->r9, regs->r10);
    rt_kprintf("fp :0x%08x ip :0x%08x\n",
               regs->fp, regs->ip);
    rt_kprintf("sp :0x%08x lr :0x%08x pc :0x%08x\n",
               regs->sp, regs->lr, regs->pc);
    rt_kprintf("spsr:0x%08x cpsr:0x%08x\n", regs->spsr, regs->cpsr);
}

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_udef(struct rt_hw_register *regs)
{
    rt_hw_show_register(regs);

    rt_kprintf("undefined instruction\n");
    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_FINSH
    list_thread();
#endif
    rt_hw_cpu_shutdown();
}

/**
 * The software interrupt instruction (SWI) is used for entering
 * Supervisor mode, usually to request a particular supervisor
 * function.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
inline void rt_hw_trap_swi(rt_uint32_t call, struct rt_hw_register *regs)
{
    extern rt_uint32_t sys_call_switch(rt_uint32_t nbr, rt_uint32_t parm1,
            rt_uint32_t parm2, rt_uint32_t parm3,
            rt_uint32_t parm4, rt_uint32_t parm5,
            rt_uint32_t parm6);
    regs->r0 = sys_call_switch(call&0xffffff,regs->r0,
            regs->r1,regs->r2,
            regs->r3,regs->r4,
            regs->r5);
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during an instruction prefetch.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_pabt(struct rt_hw_register *regs)
{
    rt_uint32_t rest;
    rt_hw_show_register(regs);

    __asm volatile("mrc p15, 0, %0, c5, c0, 1":"=r" (rest));
    rt_kprintf("prefetch abort call:%x type:%d%d%d%d\n",regs->lr,(rest>>3)&1,(rest>>2)&1,(rest>>1)&1,rest&1);
    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_MODULE
    if (rt_module_self() != RT_NULL)
        RT_ASSERT(0);
#endif

#ifdef RT_USING_FINSH
    list_thread();
#endif
    rt_hw_cpu_shutdown();
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_dabt(struct rt_hw_register *regs)
{
    rt_uint32_t addr,rest;
    rt_hw_show_register(regs);

    __asm volatile("mrc p15, 0, %0, c6, c0, 0":"=r" (addr));
    __asm volatile("mrc p15, 0, %0, c5, c0, 0":"=r" (rest));
    rt_kprintf("data abort call:%x addr:%x type:%d%d%d%d\n",regs->lr,addr,(rest>>3)&1,(rest>>2)&1,(rest>>1)&1,rest&1);
    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_MODULE
    if (rt_module_self() != RT_NULL)
        RT_ASSERT(0);
#endif

#ifdef RT_USING_FINSH
    list_thread();
#endif
    rt_hw_cpu_shutdown();
}

/**
 * Normally, system will never reach here
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_resv(struct rt_hw_register *regs)
{
    rt_hw_show_register(regs);

    rt_kprintf("not used\n");
    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_FINSH
    list_thread();
#endif
    rt_hw_cpu_shutdown();
}

extern struct rt_irq_desc irq_desc[];
extern rt_uint32_t rt_hw_interrupt_get_active(rt_uint32_t fiq_irq);
extern void rt_hw_interrupt_ack(rt_uint32_t fiq_irq, rt_uint32_t id);
    
void rt_hw_trap_irq()
{
    rt_isr_handler_t isr_func;
    rt_uint32_t irq;
    void *param;

    /* get irq number */
    irq = rt_hw_interrupt_get_active(INT_IRQ);

    /* get interrupt service routine */
    isr_func = irq_desc[irq].handler;
    param = irq_desc[irq].param;

    /* turn to interrupt service routine */
    isr_func(irq, param);

    rt_hw_interrupt_ack(INT_IRQ, irq);
#ifdef RT_USING_INTERRUPT_INFO
    irq_desc[irq].counter ++;
#endif
}

void rt_hw_trap_fiq()
{
    rt_isr_handler_t isr_func;
    rt_uint32_t irq;
    void *param;

    /* get irq number */
    irq = rt_hw_interrupt_get_active(INT_FIQ);

    /* get interrupt service routine */
    isr_func = irq_desc[irq].handler;
    param = irq_desc[irq].param;

    /* turn to interrupt service routine */
    isr_func(irq, param);

    rt_hw_interrupt_ack(INT_FIQ, irq);
#ifdef RT_USING_INTERRUPT_INFO
    irq_desc[irq].counter ++;
#endif
}
