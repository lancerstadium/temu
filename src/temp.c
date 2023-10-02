#include <stdint.h>
#include <stdbool.h>
#define OFFSET 0x088800000000ULL               
#define TO_HOST(addr) (addr + OFFSET)          
enum exit_reason_t {                           
   none,                                       
   direct_branch,                              
   indirect_branch,                            
   interp,                                     
   ecall,                                      
};                                             
typedef union {                                
    uint64_t v;                                
    uint32_t w;                                
    double d;                                  
    float f;                                   
} fp_reg_t;                                    
typedef struct {                               
    enum exit_reason_t exit_reason;            
    uint64_t reenter_pc;                       
    uint64_t gp_regs[32];                      
    fp_reg_t fp_regs[32];                      
    uint64_t pc;                               
    uint32_t fcsr;                             
} state_t;                                     
void start(volatile state_t *restrict state) { 
    uint64_t x1 = state->gp_regs[1];
    uint64_t x2 = state->gp_regs[2];
    uint64_t x8 = state->gp_regs[8];
    uint64_t x10 = state->gp_regs[10];
    uint64_t x14 = state->gp_regs[14];
    uint64_t x15 = state->gp_regs[15];
insn_101b4: {
    x15 = 122880LL;
    goto insn_101b6;
}
insn_101b6: {
    uint64_t rs1 = x15;
    x14 = rs1 + (int64_t)32LL;
    goto insn_101ba;
}
insn_101ba: {
    uint64_t rs1 = x8;
    int64_t rd = *(int64_t *)TO_HOST(rs1 + (int64_t)-24LL);
    x15 = rd;
    goto insn_101be;
}
insn_101be: {
    uint64_t rs1 = x15;
    x15 = rs1 << 2;
    goto insn_101c0;
}
insn_101c0: {
    uint64_t rs1 = x15;
    uint64_t rs2 = x14;
    x15 = rs1 + rs2;
    goto insn_101c2;
}
insn_101c2: {
    uint64_t rs1 = x15;
    int32_t rd = *(int32_t *)TO_HOST(rs1 + (int64_t)0LL);
    x15 = rd;
    goto insn_101c4;
}
insn_101c4: {
    uint64_t rs1 = x8;
    int32_t rd = *(int32_t *)TO_HOST(rs1 + (int64_t)-36LL);
    x14 = rd;
    goto insn_101c8;
}
insn_101c8: {
    uint64_t rs1 = x14;
    uint64_t rs2 = x15;
    x15 = (rs2 == 0 ? (int64_t)(int32_t)rs1 : (int64_t)(int32_t)((int64_t)(int32_t)rs1 % (int64_t)(int32_t)rs2));
    goto insn_101cc;
}
insn_101cc: {
    uint64_t rs1 = x15;
    x15 = (int64_t)(int32_t)(rs1 + (int64_t)0LL);
    goto insn_101ce;
}
insn_101ce: {
    uint64_t rs1 = x15;
    uint64_t rs2 = 0;
    if ((uint64_t)rs1 != (uint64_t)rs2) {
        goto insn_101d4;
    }
    goto insn_101d0;
}
insn_101d0: {
    uint64_t rs1 = 0;
    x15 = rs1 + (int64_t)0LL;
    goto insn_101d2;
}
insn_101d2: {
    goto insn_101fa;
}
insn_101fa: {
    uint64_t rs1 = 0;
    uint64_t rs2 = x15;
    x10 = rs1 + rs2;
    goto insn_101fc;
}
insn_101fc: {
    uint64_t rs1 = x2;
    int64_t rd = *(int64_t *)TO_HOST(rs1 + (int64_t)40LL);
    x8 = rd;
    goto insn_101fe;
}
insn_101fe: {
    uint64_t rs1 = x2;
    x2 = rs1 + (int64_t)48LL;
    goto insn_10200;
}
insn_10200: {
    uint64_t rs1 = x1;
    state->exit_reason = indirect_branch;
    state->reenter_pc = (rs1 + (int64_t)0LL) & ~(uint64_t)1;
    goto end;
}
insn_101d4: {
    uint64_t rs1 = x8;
    int64_t rd = *(int64_t *)TO_HOST(rs1 + (int64_t)-24LL);
    x15 = rd;
    goto insn_101d8;
}
insn_101d8: {
    uint64_t rs1 = x15;
    x15 = rs1 + (int64_t)1LL;
    goto insn_101da;
}
insn_101da: {
    uint64_t rs1 = x8;
    uint64_t rs2 = x15;
    *(uint64_t *)TO_HOST(rs1 + (int64_t)-24LL) = (uint64_t)rs2;
    goto insn_101de;
}
insn_101de: {
    x15 = 122880LL;
    goto insn_101e0;
}
insn_101e0: {
    uint64_t rs1 = x15;
    x14 = rs1 + (int64_t)32LL;
    goto insn_101e4;
}
insn_101e4: {
    uint64_t rs1 = x8;
    int64_t rd = *(int64_t *)TO_HOST(rs1 + (int64_t)-24LL);
    x15 = rd;
    goto insn_101e8;
}
insn_101e8: {
    uint64_t rs1 = x15;
    x15 = rs1 << 2;
    goto insn_101ea;
}
insn_101ea: {
    uint64_t rs1 = x15;
    uint64_t rs2 = x14;
    x15 = rs1 + rs2;
    goto insn_101ec;
}
insn_101ec: {
    uint64_t rs1 = x15;
    int32_t rd = *(int32_t *)TO_HOST(rs1 + (int64_t)0LL);
    x14 = rd;
    goto insn_101ee;
}
insn_101ee: {
    uint64_t rs1 = x8;
    int32_t rd = *(int32_t *)TO_HOST(rs1 + (int64_t)-36LL);
    x15 = rd;
    goto insn_101f2;
}
insn_101f2: {
    uint64_t rs1 = x15;
    x15 = (int64_t)(int32_t)(rs1 + (int64_t)0LL);
    goto insn_101f4;
}
insn_101f4: {
    uint64_t rs1 = x15;
    uint64_t rs2 = x14;
    if ((int64_t)rs1 >= (int64_t)rs2) {
        goto insn_101b4;
    }
    goto insn_101f8;
}
insn_101f8: {
    uint64_t rs1 = 0;
    x15 = rs1 + (int64_t)1LL;
    goto insn_101fa;
}
end:;
    state->gp_regs[1] = x1;
    state->gp_regs[2] = x2;
    state->gp_regs[8] = x8;
    state->gp_regs[10] = x10;
    state->gp_regs[14] = x14;
    state->gp_regs[15] = x15;
}