#define add_r6_r0_imm_256_macro asm volatile (".word 0x10000313")
#define vsetvli_r7_r6_e32_macro asm volatile (".word 0x8373d7")
#define ld_r24_a_upper_20_macro asm volatile (".word 0x2ec37")
#define add_r24_r24_imm_a_lower_12_macro asm volatile (".word 0xe50c0c13")
#define sub_r24_r24_r27_macro asm volatile (".word 0x41bc0c33")
#define ld_r25_b_upper_20_macro asm volatile (".word 0x36cb7")
#define add_r25_r25_imm_b_lower_12_macro asm volatile (".word 0xf68c8c93")
#define sub_r25_r25_r27_macro asm volatile (".word 0x41bc8cb3")
#define ld_r26_c_upper_20_macro asm volatile (".word 0x3ed37")
#define add_r26_r26_imm_c_lower_12_macro asm volatile (".word 0x870d0d13")
#define sub_r26_r26_r27_macro asm volatile (".word 0x41bd0d33")
#define ld_r31_b_row_size_upper_20_macro asm volatile (".word 0xfb7")
#define add_r31_r31_imm_b_row_size_lower_12_macro asm volatile (".word 0x160f8f93")
#define sub_r31_r31_r27_macro asm volatile (".word 0x41bf8fb3")
#define ld_r23_b_stride_upper_20_macro asm volatile (".word 0xbb7")
#define add_r23_r23_imm_b_stride_lower_12_macro asm volatile (".word 0xb00b8b93")
#define sub_r23_r23_r27_macro asm volatile (".word 0x41bb8bb3")
#define add_r25_r25_r23_macro asm volatile (".word 0x17c8cb3")
#define ld_r27_imm_0xfffff_macro asm volatile (".word 0xfffffdb7")
#define sub_r24_r24_r27_macro asm volatile (".word 0x41bc0c33")
#define sub_r25_r25_r27_macro asm volatile (".word 0x41bc8cb3")
#define sub_r26_r26_r27_macro asm volatile (".word 0x41bd0d33")
#define add_24_24_imm_32_macro asm volatile (".word 0x20c0c13")
#define add_25_25_imm_32_macro asm volatile (".word 0x20c8c93")
#define add_25_25_imm_4_macro asm volatile (".word 0x4c8c93")
#define vlwv_v1_r24_macro asm volatile (".word 0x20c6087")
#define vlwv_v2_r25_macro asm volatile (".word 0x20ce107")
#define vlwvs_v2_r25_r31_macro asm volatile (".word 0xbfce107")
#define add_22_22_imm_4_macro asm volatile (".word 0x4b0b13")
#define cp_r30_r24_macro asm volatile (".word 0xc0f13")
#define cp_r24_r30_macro asm volatile (".word 0xf0c13")
#define cp_r29_r25_macro asm volatile (".word 0xc8e93")
#define cp_r25_r29_macro asm volatile (".word 0xe8c93")
#define cp_r22_r29_macro asm volatile (".word 0xe8b13")
#define cp_r25_r22_macro asm volatile (".word 0xb0c93")
#define vmul_vv_v3_v1_v2_vm_macro asm volatile (".word 0x9620a1d7")
#define vredsum_vs_v4_v4_v3_macro asm volatile (".word 0x2322257")
#define vswv_r26_v4_macro asm volatile (".word 0x20d6227")
#define add_26_26_imm_4_macro asm volatile (".word 0x4d0d13")
#define reset_v4_r0_macro asm volatile (".word 0x5e004257")