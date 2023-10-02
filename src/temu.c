/**
 * \file src/temu.c
 * \brief temu 主程序入口
 */

#include "temu.h"

int main(int argc, char *argv[])
{
    assert(argc > 1);

    machine_t machine = {0};
    machine.cache = new_cache();                // 初始化cache
    machine_load_program(&machine, argv[1]);    // 加载可执行文件
    machine_setup(&machine, argc, argv);        // 虚拟机初始化

    while(true) {
        // 执行指令
        enum exit_reason_t reason = machine_step(&machine);
        assert(reason == ecall);
        // 获取系统调用编号：存储在通用寄存器 a7 里
        u64 syscall = machine_get_gp_reg(&machine, a7);
        // 执行系统调用
        u64 ret = do_syscall(&machine, syscall);
        // 保存系统调用返回值：返回到通用寄存器 a0 里
        machine_set_gp_reg(&machine, a0, ret);
    }
    return 0;
}