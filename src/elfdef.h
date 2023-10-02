#include "types.h"


#define EI_NIDENT 16

/// ELF 魔法数：用于识别文件类型
#define ELFMAG "\177ELF"

#define EM_RISCV 243

#define EI_CLASS     4

// e_ident 第四个字节 EI_CLASS 记录ELF文件类型
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2
#define ELFCLASSNUM  3

#define PT_LOAD 1

#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4


#define R_X86_64_PC32 2


/// @brief ELF header 结构体
typedef struct {
    u8 e_ident[EI_NIDENT];  // 文件标识符
    u16 e_type;
    u16 e_machine;      // CPU架构
    u32 e_version;
    u64 e_entry;        // 程序入口地址
    u64 e_phoff;        // program header偏移量
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;    // program header大小
    u16 e_phnum;        // program header数量
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} elf64_ehdr_t;

/// @brief Program header 结构体
typedef struct {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
} elf64_phdr_t;

typedef struct {
    u32 sh_name;
    u32 sh_type;
    u32 sh_flags;
    u64 sh_addr;
    u64 sh_offset;
    u64 sh_size;
    u32 sh_link;
    u32 sh_info;
    u64 sh_addralign;
    u64 sh_entsize;
} elf64_shdr_t;

typedef struct {
	u32 st_name;
	u8  st_info;
	u8  st_other;
	u16 st_shndx;
	u64 st_value;
	u64 st_size;
} elf64_sym_t;

typedef struct {
    u64 r_offset;
    u32 r_type;
    u32 r_sym;
    i64 r_addend;
} elf64_rela_t;