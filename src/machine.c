/**
 * \file src/machine.c
 * \brief 虚拟机操作
 */

#include "temu.h"

enum exit_reason_t machine_step(machine_t *m)
{
    while (true) // 虚拟机外层循环
    {
        bool hot = true;
        // 查找 cache 里有没有当前 pc 的可执行内存
        u8 *code = cache_lookup(m->cache, m->state.pc);
        if (code == NULL)
        {
            hot = cache_hot(m->cache, m->state.pc);     // 判断是否热代码
            if (hot)
            {                                            // 如果是热代码，则生成代码
                str_t source = machine_genblock(m);     // 生成代码块
                code = machine_compile(m, source);      // 编译代码块
            }
        }

        if (!hot)
        {   // 设置代码块解释执行
            code = (u8 *)exec_block_interp;
        }

        while (true) // 虚拟机内层循环
        {
            // 设置跳出内循环原因为 none
            m->state.exit_reason = none;
            // 执行代码块
            ((exec_block_func_t)code)(&m->state);
            // 确保跳出原因非 none
            assert(m->state.exit_reason != none);

            // 处理跳转事件：就是这里很快
            if (m->state.exit_reason == indirect_branch ||
                m->state.exit_reason == direct_branch)
            {
                // 在 cache 中寻找热门代码块
                code = cache_lookup(m->cache, m->state.reenter_pc);
                if (code != NULL) continue;
            }

            // 处理复杂指令：解释执行
            if (m->state.exit_reason == interp) {
                // 设置PC值：从这里继续执行
                m->state.pc = m->state.reenter_pc;
                // 设置代码块解释执行
                code = (u8 *)exec_block_interp;
                continue;
            }

            break;
        }
        // 更新 pc 值
        m->state.pc = m->state.reenter_pc;
        switch (m->state.exit_reason)
        {
        case direct_branch:
        case indirect_branch:
            // continue execution
            break;
        case ecall:
            return ecall;
        default:
            unreachable();
        }
    }
}

void machine_load_program(machine_t *machine, char *prog)
{
    int fd = open(prog, O_RDONLY); // 只读打开文件
    if (fd == -1)
    { // 文件名错误：输出错误信息
        fatal(strerror(errno));
    }

    // 使用文件描述符将文件加载到内存
    mmu_load_elf(&machine->mmu, fd);
    close(fd); // 关闭文件流

    // 设置程序计数器入口地址
    machine->state.pc = machine->mmu.entry;
}

void machine_setup(machine_t *m, int argc, char *argv[])
{
    size_t stack_size = 32 * 1024 * 1024; // 32MB 栈
    u64 stack = mmu_alloc(&m->mmu, stack_size);
    m->state.gp_regs[sp] = stack + stack_size; // 栈指针寄存器
    m->state.gp_regs[sp] -= 8;                 // auxv
    m->state.gp_regs[sp] -= 8;                 // envp
    m->state.gp_regs[sp] -= 8;                 // argv end
    u64 args = argc - 1;
    for (int i = args; i > 0; i--)
    {
        size_t len = strlen(argv[i]);
        u64 addr = mmu_alloc(&m->mmu, len + 1);
        mmu_write(addr, (u8 *)argv[i], len); // 将参数数据存到heap上
        m->state.gp_regs[sp] -= 8;           // argv[i]
        // 将 addr 地址值存入寄存器指向的地址（取地址的地址）
        mmu_write(m->state.gp_regs[sp], (u8 *)&addr, sizeof(u64));
    }
    m->state.gp_regs[sp] -= 8; // argc
    mmu_write(m->state.gp_regs[sp], (u8 *)&argc, sizeof(u64));
}