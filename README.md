1 可执行文件加载

./playground/riscv64/bin/riscv64-unknown-elf-readelf -a playground/prime.rv64 | grep Entry


host program <> high

guest program <> low

2 解码


while(true) {
    raw = fetch(pc);
    decode(raw);
    exec(insn);
    pv += rvc ? 2 : 4;
}


3 解释器



4 栈空间

MMU
```
[     program     ][   stack   ][    heap   ]
                                ^ alloc
sp -> stack top
stack size: 32M

[        stack        | argc argv envp auxv ]
                      ^ stack botton (sp)

```


5 系统调用

```
a7 syscall number
a0-a6 syscall parameters

```


6 JIT (Just In Time)

- 动态生成可以被执行的机器代码
- mmap -> 可读可写可执行内存
- function pointer
- function call


1. cache 类似于哈希表
2. codegen 
3. compile