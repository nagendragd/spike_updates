#include <stdio.h>
#include <rv32_vec_ins.h>
#ifndef GENERATE_ONLY
#include <generated_macros.h>
#endif
#include <stdlib.h>
#include <string.h>
#ifndef RISCV
#include <stdint.h>
#endif
//#include <context_sr_macros.h>
//#include <setup.h>

const int v_size = 8;
int nr_a, nr_b, nc_a, nc_b;

bool tooLarge (int nr, int nc)
{
   // we can not handle matrices that require more than 1GB mem
   // 1GB is 2^30 == > 2^28 4-byte elements.

   unsigned long int ne=nr*nc;
   if (ne >= 1073741824) return true;
   return false;
}

void handleError(int nr, int nc)
{
   printf("Size of matrix exceeds simulator capacity!\n");
   exit(0);
}

void loadConstantToReg(char * name, int address, int reg, int reg2, int*lower_12);


void parseTAMU(FILE * fp, int **m, int *nr, int *nc)
{
    char line[1024];
    int matrix_sz_found=0;
    int num_r, num_c, num_nz;
    while (fgets(&line[0], 1024, fp))
    {
        if (line[0] == '%') continue;
        if (matrix_sz_found) {
            int r, c, v_i;
            float v_f;
            sscanf(line, "%d %d %f", &r, &c, &v_f);
            if ((v_f < 0.0001) && (v_f > -0.9999)) v_f = 1.0;
            v_i = (int) v_f;
            (*m)[(r-1)*num_c+(c-1)]=v_i;
        } else {
            sscanf(line, "%d %d %d\n", &num_r, &num_c, &num_nz);
            if (num_r % v_size) num_r += (v_size - (num_r % v_size));
            if (num_c % v_size) num_c += (v_size - (num_c % v_size));
            matrix_sz_found = 1;
            if (tooLarge(num_r, num_c)) handleError(num_r, num_c);
            *m = (int*)malloc(num_r*num_c*sizeof(int));
            for (int i=0;i<num_r;i++) 
            for (int j=0;j<num_c;j++)
                (*m)[i*num_c + j]=0;
            *nr = num_r;
            *nc = num_c;
            printf("Matrix dimensions %d by %d\n", num_r, num_c);
        }
    } 
}

uint64_t read_cycles(void)
{
  uint64_t cycles = 0;
  asm volatile ("rdcycle %0" : "=r" (cycles));
  return cycles;
}

// output matrix C
int *c=0;
int c_lower_12;
int c_upper_20;
// CSR representation of matrix C.
int * c_rows;
int * c_cols;
int * c_vals;
int c_nnz=0;

// CSR representation of matrix A.
char a_file_name[1024];
int *a=0;
int a_lower_12;
int a_upper_20;
int * a_rows;
int * a_cols;
int * a_vals;
int a_nnz=0;

// CSR representation of matrix B.
char b_file_name[1024];
int *b=0;
int b_lower_12;
int b_upper_20;
int b_row_size_lower_12;
int b_row_size_upper_20;
int b_stride_lower_12;
int b_stride_upper_12;
int * b_rows;
int * b_cols;
int * b_vals;
int b_nnz=0;

// B is transposed and stored in CSC format
int * bt_rows;
int * bt_cols;
int * bt_vals;
int bt_nnz=0;

int t; // just for verification
volatile int last_op;

void freeMem(void)
{
    free(a); 
    free(b); 
    free(c); 
    free(a_rows); 
    free(a_cols); 
    free(a_vals); 
    free(b_rows); 
    free(b_cols); 
    free(b_vals); 
}

void initSparse(int r, int c, int * m, int *rows, int*cols, int* vals, int *g_nnz)
{
    int k=0;
    int nnz=0;
    rows[0]=0;
    *g_nnz=0;
    for (int i=0;i<r;i++)
    {
       nnz=0;
       for (int j=0;j<c;j++)
       {
           if (m[i*c+j] != 0) {
              cols[k] = j;
              vals[k] = m[i*c+j];
              k++;
              nnz++;
              (*g_nnz)++;
           }  
       }
       while (nnz % 8) {
          cols[k] = c-1;
          vals[k] = 0;
          k++;
          nnz++;
       }
       rows[i+1]=rows[i]+nnz;
    }
}

void initTransposeSparse(int r, int c, int * m, int *cols, int*rows, int* vals, int *g_nnz)
{
    // m is a matrix organized in row major order.
    // generate a CSC representation of it.
    int k=0;
    int nnz=0;
    cols[0]=0;
    *g_nnz=0;
    for (int i=0;i<c;i++)
    {
       nnz=0;
       for (int j=0;j<r;j++)
       {
           if (m[j*c+i] != 0) {
              rows[k] = j;
              vals[k] = m[j*c+i];
              k++;
              nnz++;
              (*g_nnz)++;
           }  
       }
       while (nnz % 8) {
          rows[k] = r;
          vals[k] = 0;
          k++;
          nnz++;
       }
       cols[i+1]=cols[i]+nnz;
    }
}

void execSparseScalar(void)
{
    // this is traditional row-column MAC
    // for A, we go by rows using CSR
    // for B, we go by columns using CSC

    // we must just make sure to match the 
    // indices of i-th row and j-th col.
    int nnz_a;
    int nnz_bt;
    int pos_a;
    int pos_b;
    c_nnz = 0;

    for (int i=0;i<nr_a;i++)
    {
        nnz_a = a_rows[i+1] - a_rows[i]; 
        c_rows[i] = c_nnz;
        for (int j=0;j<nc_b;j++)
        {
            //c[i*nc_b+j] = 0;
            nnz_bt = bt_rows[j+1] - bt_rows[j]; 


            // go over indices of row of A
            // see if matching index is found in column of B
            // do this efficiently
            pos_a = a_rows[i];
            pos_b = bt_rows[j];
            bool atleast_once = 0;
            int tmp=0;
            while (pos_a < a_rows[i+1])
            {
                while ((pos_b < bt_rows[j+1]) && (bt_cols[pos_b] < a_cols[pos_a])) {
                    pos_b++; 
                }
                if (bt_cols[pos_b] == a_cols[pos_a]) {
                    atleast_once = 1;
                    // c[i*nc_b+j] += bt_vals[pos_b]*a_vals[pos_a];
                    tmp += bt_vals[pos_b]*a_vals[pos_a];
                }
                pos_a++;
            }
            if (atleast_once) {
                c_cols[c_nnz] = j;
                c_vals[c_nnz] = tmp;
                c_nnz++;
            }
        }
    }
    c_rows[nr_a] = c_nnz;
}

void execSparseScalarGustavson()
{
    int nnz_a;
    int nnz_b;
    int col_a;
    int a_idx;
    int b_idx;

    for (int i=0;i<nr_a;i++)
    {
        nnz_a = a_rows[i+1] - a_rows[i]; 
        for (int k=0; k<nnz_a; k++)
        {
            a_idx = k + a_rows[i];
            col_a = a_cols[a_idx]; // a[i][a_cols[k]]; 
            
            nnz_b = b_rows[col_a + 1] - b_rows[col_a];
            for (int j=0;j<nnz_b;j++)
            {
                b_idx = j + b_rows[col_a];
                c[i*nc_b + b_cols[b_idx]] += a_vals[a_idx]*b_vals[b_idx];
            }
        }
    }

    // if we want sparse output, then dense c[] must be converted
    initSparse(nr_a, nc_b, c, c_rows, c_cols, c_vals, &c_nnz);
}

void genSparseVector(void)
{
}

void loadConstantToReg(char * name, int address, int reg, int reg2, int*var_lower_12)
{
    int upper_20 = address;
    int lower_12 = address;
    upper_20 = upper_20 >> 12;
    lower_12 = lower_12 & 0xfff;
    *var_lower_12 = lower_12;
    char str[1024];
    sprintf(&str[0], "ld_r%d_%s_upper_20_macro", reg, name);
    generateAsm(str, LD_IMM(reg, upper_20));
    sprintf(&str[0], "add_r%d_r%d_imm_%s_lower_12_macro", reg, reg, name);
    generateAsm(str, ADD_IMM(reg, reg, lower_12));
    sprintf(&str[0], "sub_r%d_r%d_r%d_macro", reg, reg, reg2);
    generateAsm(&str[0], SUB_R_R(reg, reg, reg2));
}

void genHWHelper(void)
{
}

#ifndef GENERATE_ONLY
void execSparseVector(void)
{
}

void execHWHelper(void)
{
    c_nnz = 0;

    for (int i=0;i<nr_a;i++)
    {
        c_rows[i] = c_nnz;
        for (int j=0;j<nc_b;j++)
        {
            c[i*nc_b+j] = 0;

            bool flag = READ_FLAG_FROM_BUFFER_CONTROLLER;
            // flag == true ==> data available for row i of A and col j of B
            // flag == false ==> end of data
            bool atleast_once = 0;
            while (flag) 
            {
                VLOAD_BUF1_TO_V1; // BUF1 to values in row i of A
                VLOAD_BUF2_TO_V2; // BUF2 to values in col j of B
                VMUL_V3_V1_V2; // V3 = V1*V2 element-wise
                VRED_V4_V3; // v4[0] = v4[0] + sum of elements of v3[.]
                atleast_once = 1;
                flag = READ_FLAG_FROM_BUFFER_CONTROLLER;
            }
            if (atleast_once) 
            {
                c_cols[c_nnz] = j;
                c_vals[c_nnz] = V4[0];
                c_nnz+=1;
                V4[0] = 0; // clear for next iteration
            }
        }
    }
    c_rows[nr_a] = c_nnz;
}

void execDenseVector(void)
{
    // VL: 256 bits
    add_r6_r0_imm_256_macro;

    // SEW: 32 bits
    vsetvli_r7_r6_e32_macro;

    // init R27 to 0xfffff
    ld_r27_imm_0xfffff_macro;

    // load A to R24
    ld_r24_a_upper_20_macro;
    add_r24_r24_imm_a_lower_12_macro;
    if (a_lower_12 & 0x800) sub_r24_r24_r27_macro;

    // load B to R25
    ld_r25_b_upper_20_macro;
    add_r25_r25_imm_b_lower_12_macro;
    if (b_lower_12 & 0x800) sub_r25_r25_r27_macro;

    // load B's row size (bytes) to R31
    ld_r31_b_row_size_upper_20_macro;
    add_r31_r31_imm_b_row_size_lower_12_macro;
    if (b_row_size_lower_12 & 0x800) sub_r31_r31_r27_macro;

    // load B's stride (bytes) to R23
    ld_r23_b_stride_upper_20_macro;
    add_r23_r23_imm_b_stride_lower_12_macro;
    if (b_stride_lower_12 & 0x800) sub_r23_r23_r27_macro;

    // save B to R29
    cp_r29_r25_macro;

    // load C to R26
    ld_r26_c_upper_20_macro;
    add_r26_r26_imm_c_lower_12_macro;
    if (c_lower_12 & 0x800) sub_r26_r26_r27_macro;

    for (int i=0;i<nr_a;i++)
    {
        // save present A address to R30
        cp_r30_r24_macro;

        // load base of B back from R29
        // cp_r25_r29_macro;

        // load base of B back into R22
        // we use R22 to move by columns of B
        // R25 holds the current address in B we are loading from
        cp_r22_r29_macro;

        for (int j=0;j<nc_b;j++)
        {
            // c[i*nc_b+j] = 0;

            // reset A to row i
            cp_r24_r30_macro;

            // reset B to col j
            cp_r25_r22_macro;

            for (int k=0;k<nr_b; k+=8)
            {
                //c[i*nc_b+j] += a[i*nc_a+k]*b[k*nr_b+j];
    
                // load vector from address of A into V1
                vlwv_v1_r24_macro;

                // load vector -strided from address of B into V2
                vlwvs_v2_r25_r31_macro;
                //vlwv_v2_r25_macro;

                // pairwise multiply
                vmul_vv_v3_v1_v2_vm_macro;

                // reduce
                vredsum_vs_v4_v4_v3_macro;

                // increment A by vector size
                add_24_24_imm_32_macro;

                // increment B by VLEN * stride
                add_r25_r25_r23_macro;
            }
            // store to C
            vswv_r26_v4_macro;

            // advance C by 4 bytes
            add_26_26_imm_4_macro;

            // clear cumulator v4
            reset_v4_r0_macro;

            // move B to next column.
            // this is just loading current B
            // adding 4
            add_22_22_imm_4_macro;
        }

        // move A to next row i -- which is just adding 32B to where we are
        add_24_24_imm_32_macro;
    }
}

#endif

void execMatMult(void)
{
    for (int i=0;i<nr_a;i++)
    {
        for (int j=0;j<nc_b;j++)
        {
            c[i*nc_b+j] = 0;
            for (int k=0;k<nr_b; k++)
            {
                c[i*nc_b+j] += a[i*nc_a+k]*b[k*nr_b+j];
            }
        }
    }
}

void clearC(void)
{
    for (int i=0;i<nr_a;i++)
    {
        for (int j=0;j<nc_b;j++)
        {
            c[i*nc_b + j] = 0;
        }
    }
}

void execDenseScalar(void)
{
    // gustavson's works by pairwise multiplying 
    // and accumulating partial sums of rows.
    for (int i=0;i<nr_a;i++)
    {
        for (int j=0;j<nr_b;j++)
        {
            // i-th row of A and j-th row of B
            // multiply element A[i][j] with each element of B[j]
            // and accumulate product into corresponding element of C[i]
            for (int k=0;k<nc_b;k++)
            {
                c[i*nc_b + k] += a[i*nc_a + j]*b[j*nc_b + k]; 
            }
        }
    }
}

void genDenseVector(void)
{
    printf("A matrix is at %p\n", a);
    printf("B matrix is at %p\n", b);
    printf("C matrix is at %p\n", c);
    generateAsm("add_r6_r0_imm_256_macro", ADD_IMM(R6,R0,256));
    generateAsm("vsetvli_r7_r6_e32_macro", VSETVLI(R7, R6, (E32 << VSEW_OFFSET)));

    // load address of A into R24
    loadConstantToReg("a", (int)a, R24, R27, &a_lower_12);
    // load address of B into R25
    loadConstantToReg("b", (int)b, R25, R27, &b_lower_12);
    // load address of C into R26
    loadConstantToReg("c", (int)c, R26, R27, &c_lower_12);

    // load row size of B into R31
    loadConstantToReg("b_row_size", nc_b*4, R31, R27, &b_row_size_lower_12);

    // load VLEN * row size of B into R23
    loadConstantToReg("b_stride", nc_b*4*v_size, R23, R27, &b_stride_lower_12);

    // move B's present access point by stride
    generateAsm("add_r25_r25_r23_macro", ADD_R_R(R25, R25, R23));

    // load 0xfffff into R27 for address constant adjustment
    generateAsm("ld_r27_imm_0xfffff_macro", LD_IMM(R27, 0xfffff));
    generateAsm("sub_r24_r24_r27_macro", SUB_R_R(R24,R24,R27));
    generateAsm("sub_r25_r25_r27_macro", SUB_R_R(R25,R25,R27));
    generateAsm("sub_r26_r26_r27_macro", SUB_R_R(R26,R26,R27));

    // increment addresses A, B by 32 bytes.
    generateAsm("add_24_24_imm_32_macro", ADD_IMM(R24,R24,0b000000100000));
    generateAsm("add_25_25_imm_32_macro", ADD_IMM(R25,R25,0b000000100000));
    generateAsm("add_25_25_imm_4_macro", ADD_IMM(R25,R25,0b000000000100));

    // load vector from address of A into V1
    generateAsm("vlwv_v1_r24_macro", VLWV(V1,R24));

    // load vector from address of B into V2
    generateAsm("vlwv_v2_r25_macro", VLWV(V2,R25));

    // load vector -strided from address of B into V2
    generateAsm("vlwvs_v2_r25_r31_macro", VLWVS(V2,R25,R31));

    // Move B to next column
    generateAsm("add_22_22_imm_4_macro", ADD_IMM(R22, R22, 4));

    // generate a copy instruction to save address of A
    generateAsm("cp_r30_r24_macro", ADD_IMM(R30, R24, 0));
    // generate a copy instruction to restore address of A
    generateAsm("cp_r24_r30_macro", ADD_IMM(R24, R30, 0));

    // generate a copy instruction to save address of B
    generateAsm("cp_r29_r25_macro", ADD_IMM(R29, R25, 0));
    // generate a copy instruction to restore address of B
    generateAsm("cp_r25_r29_macro", ADD_IMM(R25, R29, 0));
    // generate a copy instruction to restore address of B
    generateAsm("cp_r22_r29_macro", ADD_IMM(R22, R29, 0));

    // restore to current B pointer from saved column pointer
    generateAsm("cp_r25_r22_macro", ADD_IMM(R25, R22, 0));

    // multiply pairwise V1 and V2
    generateAsm("vmul_vv_v3_v1_v2_vm_macro", VMUL_VV(V3,V1,V2,VM));

    // use V4 to hold cumulation
    generateAsm("vredsum_vs_v4_v4_v3_macro", VREDSUM_VS(V4, V4, V3, VM));

    // store V4 to address of C
    generateAsm("vswv_r26_v4_macro", VSWV(R26,V4));

    // increment address register holding address of C by 4 bytes
    generateAsm("add_26_26_imm_4_macro", ADD_IMM(R26,R26,4));

    // clear cumulator V4[0]
    generateAsm("reset_v4_r0_macro", VMV_VX(V4,R0));
}

void usage(void)
{
    printf("Program requires five args:\n");
    printf("Arg 1: 0 or 1 value -- 0 if only compile, 1 if compile+exec\n");
    printf("Arg 2: 0 - dense matrix, 1 - CSR sparse matrix, 2 - TAMU COO sparse matrix\n");
    printf("Arg 3: matrix data file A as input.\n");
    printf("Arg 4: matrix data file B as input.\n");
    printf("Arg 5: 0 or 1 value -- 0 if doing dense, 1 if doing sparse\n");
    printf("Arg 6: 0/1/2 value -- 0 if using scalar, 1 if using vectors, 2 if using hardware helper (2 is valid only with sparse)\n");
}

typedef enum {DENSE=0, CSR=1, COO=2} matrix_type_t;

int main(int argc, char ** argv)
{
    if (argc <= 6) { usage(); return 0; }
    bool do_compile_exec = false;
    if (strcmp(argv[1],"1") == 0) do_compile_exec = true;

    FILE*a_fp = fopen(argv[3],"r");
    strcpy(a_file_name, argv[3]);
    if (!a_fp) { printf("Could not open %s for reading matrix A.\n", argv[3]); return 0;}

    FILE*b_fp = fopen(argv[4],"r");
    strcpy(b_file_name, argv[4]);
    if (!b_fp) { printf("Could not open %s for reading matrix B.\n", argv[4]); return 0;}

    matrix_type_t matrix_type;
    if (strcmp(argv[2], "0")==0) matrix_type=DENSE;
    else if (strcmp(argv[2], "1")==0) matrix_type=CSR;
    else if (strcmp(argv[2], "2")==0) matrix_type=COO;
    else { printf("Unknown matrix type.\n"); usage(); return 0;}

    if (matrix_type == DENSE) {
        fscanf(a_fp, "%d\n", &nr_a);
        nr_a = nc_a;
        fscanf(b_fp, "%d\n", &nr_b);
        nr_b = nc_b;
        if (nc_a != nr_b) {
            printf("Matrices are incompatible in size. Can't be multiplied.\n");
            exit(0);
        }
        if (tooLarge(nr_a, nc_a)) handleError(nr_a, nc_a);
        a = (int*)malloc(nr_a*nc_a*sizeof(int));
        b = (int*)malloc(nr_b*nc_b*sizeof(int));
        c = (int*)malloc((nr_a*nc_b + v_size)*sizeof(int));
        for (int i=0;i<nr_a*nc_a;i++) fscanf(a_fp,"%d\n", &a[i]);
        for (int i=0;i<nr_b*nc_b;i++) fscanf(b_fp,"%d\n", &b[i]);
    }
    else if (matrix_type == CSR) {
        printf("Format as yet unsupported\n");
        return 0;
    } else if (matrix_type == COO) {
        // Read TAMU Matrix Market formatted input
        // Assumptions: commented lines start with %
        // Matrix COO indices are index-1 based and not index-0 based
        parseTAMU(a_fp, &a, &nr_a, &nc_a);
        parseTAMU(b_fp, &b, &nr_b, &nc_b);
        if (nc_a != nr_b) 
        {
            printf("Incompatible matrices can not be multiplied!\n");
            return 0;
        }
        c = (int*)malloc((nr_a*nc_b + v_size)*sizeof(int));
    }
    a_rows = (int*) malloc((nr_a+1)*sizeof(int));
    a_cols = (int*) malloc((nr_a+8)*(nc_a+8)*sizeof(int));
    a_vals = (int*) malloc((nr_a+8)*(nc_a+8)*sizeof(int));
    b_rows = (int*) malloc((nr_b+1)*sizeof(int));
    b_cols = (int*) malloc((nr_b+8)*(nc_b+8)*sizeof(int));
    b_vals = (int*) malloc((nr_b+8)*(nc_b+8)*sizeof(int));
    bt_rows = (int*) malloc((nc_b+1)*sizeof(int));
    bt_cols = (int*) malloc((nr_b+8)*(nc_b+8)*sizeof(int));
    bt_vals = (int*) malloc((nr_b+8)*(nc_b+8)*sizeof(int));
    c_rows = (int*)malloc((nr_a+1)*sizeof(int));
    c_cols = (int*)malloc((nr_a+8)*(nc_b+8)*sizeof(int));
    c_vals = (int*)malloc((nr_a+8)*(nc_b+8)*sizeof(int));
    initSparse(nr_a, nc_a, a, a_rows, a_cols, a_vals, &a_nnz);
    initSparse(nr_b, nc_b, b, b_rows, b_cols, b_vals, &b_nnz);
    initTransposeSparse(nr_b, nc_b, b, bt_rows, bt_cols, bt_vals, &bt_nnz);

    bool do_sparse = false;
    if (strcmp(argv[5],"1") == 0) do_sparse = true;

    bool use_vectors = false;
    bool use_hw_helper = false;
    if (strcmp(argv[6],"1") == 0) use_vectors = true;
    if (strcmp(argv[6],"2") == 0) use_hw_helper = true;

    if (!do_sparse && use_hw_helper) {
        printf("Invalid options: HW Helper option is only valid with sparse computation.\n");
        exit(0);
    }
    genDenseVector();
    genSparseVector();
    genHWHelper();

    if (!do_compile_exec) { freeMem(); return 0;}
    execMatMult();

#define COMPARE (nr_a*nc_b-1)
    last_op = c[COMPARE];
    printf("last_op is %d\n", last_op);
    clearC();
    volatile uint64_t s, e;
    //save_regs();
#ifdef GENERATE_ONLY
    return 0;
#else
    s = read_cycles();
    if (do_sparse)
    {
        if (use_vectors)
        {
            execSparseVector();
        }
        else if (use_hw_helper)
        {
            execHWHelper();
        }
        else
        {
            //execSparseScalarGustavson();
            execSparseScalar();
        }
    }
    else 
    {
        if (use_vectors)
        {
            execDenseVector();
        }
        else
        {
            execDenseScalar();
        }
    }
    //restore_regs();
    e = read_cycles();
    printf("Matrix A: %s; %d by %d; Matrix B: %s; %d by %d; Cycles: %ld\n", a_file_name, nr_a, nc_a, b_file_name, nr_b, nc_b, (unsigned long ) (e-s));
    
    // check contents of y.
    printf("Last expected: %d\n", last_op);
    printf("Last output: %d\n", c[COMPARE]);
    if (last_op != c[COMPARE]) printf("Failed!\n");
    else printf("Passed!\n");

    freeMem();
    
    return 0;
#endif
}
