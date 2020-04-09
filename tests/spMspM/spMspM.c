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

void loadConstantToReg(char * name, int address, int reg, int*lower_12);


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
// CSR representation of matrix C.
int * c_rows;
int * c_cols;
int * c_vals;
int c_nnz=0;

// CSR representation of matrix A.
char a_file_name[1024];
int *a=0;
int * a_rows;
int * a_cols;
int * a_vals;
int a_nnz=0;

// CSR representation of matrix B.
char b_file_name[1024];
int *b=0;
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
            c[i*nc_b+j] = 0;
            nnz_bt = bt_rows[j+1] - bt_rows[j]; 


            // go over indices of row of A
            // see if matching index is found in column of B
            // do this efficiently
            pos_a = a_rows[i];
            pos_b = bt_rows[j];
            while (pos_a < a_rows[i+1])
            {
                while ((pos_b < bt_rows[j+1]) && (bt_cols[pos_b] < a_cols[pos_a])) {
                    pos_b++; 
                }
                if (bt_cols[pos_b] == a_cols[pos_a]) {
                    c[i*nc_b+j] += bt_vals[pos_b]*a_vals[pos_a];
                    c_cols[c_nnz] = a_cols[pos_a];
                    c_vals[c_nnz] = bt_vals[pos_b]*a_vals[pos_a];
                    c_nnz++;
                }
                pos_a++;
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

void loadConstantToReg(char * name, int address, int reg, int*var_lower_12)
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
    sprintf(&str[0], "sub_r%d_r%d_r12_macro", reg, reg);
    generateAsm(&str[0], SUB_R_R(reg, reg, R12));
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
}

void execDenseVector(void)
{
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
        c = (int*)malloc(nr_a*nc_b*sizeof(int));
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
        c = (int*)malloc(nr_a*nc_b*sizeof(int));
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
