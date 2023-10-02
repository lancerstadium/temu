
# 展示

```
~/items/temu$ make
clang -O3 -Wall -Werror -Wimplicit-fallthrough -c -o obj/codegen.o src/codegen.c
clang -O3 -Wall -Werror -Wimplicit-fallthrough -lm -o temu obj/cache.o obj/codegen.o obj/compile.o obj/decode.o obj/interp.o obj/machine.o obj/mmu.o obj/set.o obj/stack.o obj/str.o obj/syscall.o obj/temp.o obj/temu.o 

~/items/temu$ time ./temu ./playground/prime.rv64
104729
./temu ./playground/prime.rv64  0.56s user 0.05s system 99% cpu 0.609 total

~/items/temu$ time ./playground/riscv64/bin/qemu-riscv64 ./playground/prime.rv64       
104729
./playground/riscv64/bin/qemu-riscv64 ./playground/prime.rv64  2.29s user 0.01s system 99% cpu 2.316 total

```


## 1 可执行文件加载

```
./playground/riscv64/bin/riscv64-unknown-elf-readelf -a playground/prime.rv64 | grep Entry

```

```
host program <> high
guest program <> low
```


## 2 解码

``` c
while(true) {
    raw = fetch(pc);
    decode(raw);
    exec(insn);
    pv += rvc ? 2 : 4;
}
```


## 3 解释器



## 4 栈空间

MMU
```
+ sp -> stack top
+ stack size: 32M

[     program     ][   stack   ][    heap   ]
                                ^ alloc


[        stack        | argc argv envp auxv ]
                      ^ stack botton (sp)

```


## 5 系统调用

```
a7 syscall number
a0-a6 syscall parameters

```

## 6 JIT (Just In Time) 优化

- 动态生成可以被执行的机器代码
- mmap -> 可读可写可执行内存
- function pointer
- function call


1. cache 类似于哈希表
2. codegen 
3. compile