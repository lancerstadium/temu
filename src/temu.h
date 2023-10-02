#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "types.h"
#include "elfdef.h"
#include "reg.h"


#define ROUNDDOWN(x, k) ((x) & -(k))
#define ROUNDUP(x, k)   (((x) + (k)-1) & -(k))
#define MIN(x, y)       ((y) > (x) ? (x) : (y))
#define MAX(x, y)       ((y) < (x) ? (x) : (y))

/// 高位 + 偏移量 -> 低位
#define GUEST_MEMORY_OFFSET 0x088800000000ULL

#define TO_HOST(addr)  (addr + GUEST_MEMORY_OFFSET)
#define TO_GUEST(addr) (addr - GUEST_MEMORY_OFFSET)

/// 数组大小
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))

#define fatalf(fmt, ...) (fprintf(stderr, "fatal: %s:%d " fmt "\n", __FILE__, __LINE__, __VA_ARGS__), exit(1))
/// fatal 宏：输出错误信息
#define fatal(msg) fatalf("%s", msg)
/// todo 宏：输出待办信息
#define todo(msg) (fprintf(stderr, "warning: %s:%d [TODO] %s\n", __FILE__, __LINE__, msg))
/// unreachable 宏：输出不可达信息
#define unreachable() (fatal("unreachable"), __builtin_unreachable())


// ============================================================================== //
// 指令 insn
// ============================================================================== //

/// @brief 指令类型
enum insn_type_t {
    insn_lb, insn_lh, insn_lw, insn_ld, insn_lbu, insn_lhu, insn_lwu,
    insn_fence, insn_fence_i,
    insn_addi, insn_slli, insn_slti, insn_sltiu, insn_xori, insn_srli, insn_srai, insn_ori, insn_andi, insn_auipc, insn_addiw, insn_slliw, insn_srliw, insn_sraiw,
    insn_sb, insn_sh, insn_sw, insn_sd,
    insn_add, insn_sll, insn_slt, insn_sltu, insn_xor, insn_srl, insn_or, insn_and,
    insn_mul, insn_mulh, insn_mulhsu, insn_mulhu, insn_div, insn_divu, insn_rem, insn_remu,
    insn_sub, insn_sra, insn_lui,
    insn_addw, insn_sllw, insn_srlw, insn_mulw, insn_divw, insn_divuw, insn_remw, insn_remuw, insn_subw, insn_sraw,
    insn_beq, insn_bne, insn_blt, insn_bge, insn_bltu, insn_bgeu,
    insn_jalr, insn_jal, insn_ecall,
    insn_csrrc, insn_csrrci, insn_csrrs, insn_csrrsi, insn_csrrw, insn_csrrwi,
    insn_flw, insn_fsw,
    insn_fmadd_s, insn_fmsub_s, insn_fnmsub_s, insn_fnmadd_s, insn_fadd_s, insn_fsub_s, insn_fmul_s, insn_fdiv_s, insn_fsqrt_s,
    insn_fsgnj_s, insn_fsgnjn_s, insn_fsgnjx_s,
    insn_fmin_s, insn_fmax_s,
    insn_fcvt_w_s, insn_fcvt_wu_s, insn_fmv_x_w,
    insn_feq_s, insn_flt_s, insn_fle_s, insn_fclass_s,
    insn_fcvt_s_w, insn_fcvt_s_wu, insn_fmv_w_x, insn_fcvt_l_s, insn_fcvt_lu_s,
    insn_fcvt_s_l, insn_fcvt_s_lu,
    insn_fld, insn_fsd,
    insn_fmadd_d, insn_fmsub_d, insn_fnmsub_d, insn_fnmadd_d,
    insn_fadd_d, insn_fsub_d, insn_fmul_d, insn_fdiv_d, insn_fsqrt_d,
    insn_fsgnj_d, insn_fsgnjn_d, insn_fsgnjx_d,
    insn_fmin_d, insn_fmax_d,
    insn_fcvt_s_d, insn_fcvt_d_s,
    insn_feq_d, insn_flt_d, insn_fle_d, insn_fclass_d,
    insn_fcvt_w_d, insn_fcvt_wu_d, insn_fcvt_d_w, insn_fcvt_d_wu,
    insn_fcvt_l_d, insn_fcvt_lu_d,
    insn_fmv_x_d, insn_fcvt_d_l, insn_fcvt_d_lu, insn_fmv_d_x,
    num_insns,
};

/// @brief 指令结构体
typedef struct {
    i8 rd;      // 操作码
    i8 rs1;     // 寄存器1
    i8 rs2;     // 寄存器2
    i8 rs3;     // 寄存器3
    i16 csr;    // csr数
    i32 imm;    // 立即数
    enum insn_type_t type;  // 指令类型
    bool rvc;   // 是否 rvc 压缩指令
    bool cont;  // 是否继续执行
} insn_t;

/// @brief 解码指令
/// @param insn 指令
/// @param data 数据
void insn_decode(insn_t *insn, u32 data);


// ============================================================================== //
// 栈 stack => stack.c
// ============================================================================== //

#define STACK_CAP 256

/// @brief 栈结构体
typedef struct {
    i64 top;
    u64 elems[STACK_CAP];
} stack_t;

/// @brief 入栈操作
/// @param stack 栈
/// @param element 元素
void stack_push(stack_t * stack, u64 element);

/// @brief 出栈操作
/// @param stack 栈
/// @param elem_ptr 接收元素的指针
/// @return 是否成功出栈
bool stack_pop(stack_t * stack, u64 * elem_ptr);

/// @brief 重置栈
/// @param stack 栈
void stack_reset(stack_t * stack);

/// @brief 打印栈信息
/// @param stack 栈
void stack_print(stack_t * stack);


// ============================================================================== //
// 字符串 str => str.c
// ============================================================================== //

#define STR_MAX_PREALLOC (1024 * 1024)
#define STRHDR(s) ((strhdr_t *)((s)-(sizeof(strhdr_t))))

#define DECLEAR_STATIC_STR(name)   \
    static str_t name = NULL;  \
    if (name) str_clear(name); \
    else name = str_new();     \

/// 等于 char * 类型
typedef char * str_t;

typedef struct {
    u64 len;
    u64 alloc;
    char buf[];
} strhdr_t;

/// @brief 新建字符串
/// @return 字符串对象
inline str_t str_new() {
    strhdr_t *h = (strhdr_t *)calloc(1, sizeof(strhdr_t));
    return h->buf;
}

/// @brief 查看字符串长度
/// @param str 字符串对象
/// @return 长度
inline size_t str_len(const str_t str) {
    return STRHDR(str)->len;
}

/// @brief 清除字符串
/// @param str 字符串对象
void str_clear(str_t str);

/// @brief 从字符串后新增字符
/// @param str 字符串对象
/// @param t 新增字符
/// @return 新增后的字符串对象
str_t str_append(str_t str, const char *t);


// ============================================================================== //
// 集合 set => set.c
// ============================================================================== //

#define SET_SIZE (32 * 1024)

/// @brief 集合结构体
typedef struct {
    u64 table[SET_SIZE];
} set_t;

/// @brief 判断集合内是否有该元素
/// @param set 集合对象
/// @param element 元素
/// @return `true or false`
bool set_has(set_t *set, u64 element);

/// @brief 在集合内新增元素
/// @param set 集合对象
/// @param element 元素
/// @return `true or false`是否新增成功
bool set_add(set_t *set, u64 element);

/// @brief 重置集合内元素
/// @param set 集合对象
void set_reset(set_t *set);


// ============================================================================== //
// 内存 mmu
// ============================================================================== //

/// @brief 内存信息结构体
typedef struct {
    u64 entry;          // 入口地址
    u64 host_alloc;     // 程序内存分割值：最大segament
    u64 alloc;          // 申请内存地址
    u64 base;           //

    //              | host_alloc
    // [   `ELF`    |    `malloc`   |  ... ]
    //              | base          |alloc
} mmu_t;

/// @brief 将文件读入内存
/// @param mmu 内存对象
/// @param fd 文件描述符
void mmu_load_elf(mmu_t *mmu, int fd);

/// @brief 内存申请
/// @param mmu 内存对象
/// @param sz 申请大小
/// @return base地址
u64 mmu_alloc(mmu_t *mmu, i64 sz);


/// @brief 将长度为 len 的 data 数据存入指定内存地址 addr
/// @param addr 地址
/// @param data 数据
/// @param len 数据长度
inline void mmu_write(u64 addr, u8 *data, size_t len) {
    memcpy((void *)TO_HOST(addr), (void *)data, len);
}

// ============================================================================== //
// 高速缓存 cache
// ============================================================================== //

/// 代码块最大存储个数
#define CACHE_ENTRY_SIZE (64 * 1024)
/// 高速缓存大小：64MB
#define CACHE_SIZE       (64 * 1024 * 1024)

/// @brief 高速缓存表项
typedef struct {
    u64 pc;         // 指令计数器  key
    u64 hot;        // 热度值     flag
    u64 offset;     // 偏移量     value
} cache_item_t;

/// @brief 高速缓存结构体
typedef struct {
    u8 *jitcode;    // 可执行内存指针
    u64 offset;     // JIT code 使用地址：不回收
    cache_item_t table[CACHE_ENTRY_SIZE];   // 高速缓存表：哈希表
} cache_t;

/// @brief 将一个新的高速缓存映射到内存
/// @return 高速缓存对象
cache_t *new_cache();

/// @brief 在 cache 中寻找与 pc 相匹配的代码块
/// @param cache 高速缓存对象
/// @param pc 程序计数器
/// @return 可执行内存地址
u8 *cache_lookup(cache_t *cache, u64 pc);

/// @brief 在 cache 中加入新的热代码块
/// @param cache 高速缓存对象
/// @param pc 当前热代码块的程序计数器
/// @param code 热代码块内存地址
/// @param sz 代码块大小
/// @param align 对齐
/// @return 可执行内存地址
u8 *cache_add(cache_t *cache, u64 pc, u8 *code, size_t sz, u64 align);


/// @brief 查询当前 pc 指向的代码块是否为热代码
/// @param cache 高速缓存对象
/// @param  pc 程序计数器
/// @return `true or false`是否热代码
bool cache_hot(cache_t *cache, u64 pc);

// ============================================================================== //
// 状态 state
// ============================================================================== //

/// @brief 跳出内存循环的原因
enum exit_reason_t {
    none,               // 无
    direct_branch,      // 直接跳转
    indirect_branch,    // 间接跳转
    ecall,              // 
    interp,             // 需要解释执行：复杂指令，频率低
};

/// @brief csr寄存器
enum csr_t {
    fflags = 0x001,
    frm    = 0x002,
    fcsr   = 0x003,
};

/// @brief 状态信息结构体
typedef struct {
    enum exit_reason_t exit_reason; // 跳出循环原因
    u64 reenter_pc;                 // 再次跳入指令的pc值
    u64 gp_regs[num_gp_regs];       // 通用寄存器
    fp_reg_t fp_regs[num_fp_regs];  // 浮点型寄存器
    u64 pc;                         // 程序计数器：程序当前所在位置
} state_t;

// ============================================================================== //
// 解释器 interperter => interp.c
// ============================================================================== //

/// @brief 解释执行代码块
/// @param state 状态信息对象
void exec_block_interp(state_t *state);


// ============================================================================== //
// 虚拟机 machine => machine.c
// ============================================================================== //

/// @brief 虚拟机结构体：src/machine.c
typedef struct {
    state_t state;
    mmu_t mmu;
    cache_t *cache;
} machine_t;

/// 执行函数签名
typedef void (*exec_block_func_t)(state_t *);

/// @brief 获取通用寄存器的值
/// @param m 虚拟机对象
/// @param reg 寄存器编号
/// @return 存储值
inline u64 machine_get_gp_reg(machine_t *m, i32 reg) {
    assert(reg >= 0 && reg <= num_gp_regs);
    return m->state.gp_regs[reg];
}

/// @brief 设置通用寄存器的值
/// @param m 虚拟机对象
/// @param reg 寄存器编号
/// @param data 设置值
inline void machine_set_gp_reg(machine_t *m, i32 reg, u64 data) {
    assert(reg >= 0 && reg <= num_gp_regs);
    m->state.gp_regs[reg] = data;
}

/// @brief 虚拟机加载可执行文件
/// @param m 虚拟机对象
/// @param prog 读入可执行文件名
void machine_load_program(machine_t *m, char *prog);

/// @brief 虚拟机初始化：配置栈空间
/// @param m 虚拟机对象
/// @param argc 参数数目
/// @param argv 参数值
void machine_setup(machine_t *m, int argc, char *argv[]);

/// @brief 虚拟机执行代码，处理异常
/// @param m 虚拟机对象
enum exit_reason_t machine_step(machine_t *m);

// ============================================================================== //
// 代码生成 codegen => codegen.c
// ============================================================================== //

/// @brief 虚拟机生成中间代码
/// @param m 虚拟机对象
/// @return `str_t` 类型 C 中间代码
str_t machine_genblock(machine_t *m);

// ============================================================================== //
// 编译 compile => compile.c
// ============================================================================== //

/// @brief 虚拟机编译中间代码
/// @param m 虚拟机对象
/// @param str 中间代码
/// @return 可执行内存地址
u8 *machine_compile(machine_t *m, str_t str);

// ============================================================================== //
// 系统调用 syscall => syscall.c
// ============================================================================== //

/// @brief 执行系统调用
/// @param m 虚拟机对象
/// @param n 系统调用编号
/// @return 系统调用返回地址
u64 do_syscall(machine_t *m, u64 n);
