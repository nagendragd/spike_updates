#include <stdio.h>
#include <rv32_vec_ins.h>
#include <stdlib.h>
#include <string.h>

void genVectorCode(void)
{
    generateAsm("add_r6_r0_imm_256_macro", ADD_IMM(R6,R0,256));
    generateAsm("vsetvli_r7_r6_e32_macro", VSETVLI(R7, R6, (E32 << VSEW_OFFSET)));

    generateAsm("ld_r12_imm_0xfffff_macro", LD_IMM(R12, 0xfffff));
    generateAsm("sub_r10_r10_r12_macro", SUB_R_R(R10,R10,R12));
    generateAsm("sub_r11_r11_r12_macro", SUB_R_R(R11,R11,R12));
    generateAsm("sub_r13_r13_r12_macro", SUB_R_R(R13,R13,R12));
    generateAsm("sub_r28_r28_r12_macro", SUB_R_R(R28,R28,R12));

    generateAsm("vlwv_v10_r10_macro", VLWV(V10,R10));
    generateAsm("vlwv_v13_r13_macro", VLWV(V13,R13));
    generateAsm("vadd_vv_v11_v10_v13_vm_macro", VADD_VV(V11,V10,V13,VM));
    generateAsm("vmul_vv_v11_v10_v13_vm_macro", VMUL_VV(V11,V10,V13,VM));
    generateAsm("vswv_r11_v11_macro", VSWV(R11,V11));
    generateAsm("vswv_r28_v28_macro", VSWV(R28,V28));
    generateAsm("add_10_10_imm_256_macro", ADD_IMM(R10,R10,0b000000100000));
    generateAsm("add_11_11_imm_256_macro", ADD_IMM(R11,R11,0b000000100000));
    generateAsm("add_11_11_imm_32_macro", ADD_IMM(R11,R11,0b000000000100));
    generateAsm("add_13_13_imm_256_macro", ADD_IMM(R13,R13,0b000000100000));
    generateAsm("add_28_28_imm_256_macro", ADD_IMM(R28,R28,0b000000100000));
    generateAsm("add_28_28_imm_32_macro", ADD_IMM(R28,R28,0b000000000100));
    // use V28 to hold cumulation
    generateAsm("vredsum_vs_v28_v28_v11_macro", VREDSUM_VS(V28, V28, V11, VM));

    // generate a copy instruction to save contents of R13
    generateAsm("cp_r29_r13_macro", ADD_IMM(R29, R13, 0));
    // generate a copy instruction to restore contents to R13
    generateAsm("cp_r13_r29_macro", ADD_IMM(R13, R29, 0));

    // generate instruction to clear cumulator V28[0]
    generateAsm("reset_v28_r0_macro", VMV_VX(V28,R0));

    // generate instruction to save V28[0] into x register R30.
    generateAsm("move_r30_v28_macro", VMV_XV(R30,V28));

    // generate instruction to save R30 to MEM[R11].
    generateAsm("sw_r11_r30_macro", STW(R11, R30, 0));
}

int main(int argc, char ** argv)
{
    genVectorCode();

    return 0;
}
