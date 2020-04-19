#include <iostream> 
#include <string.h>
using namespace std;

const int n_args = 4;

void usage(void)
{
    printf("The program requires %d arguments:\n", n_args);
    printf("Length and breadth of matrix to be read\n");
    printf("Input weight matrix file\n");
    printf("Sparsified quantized output weight matrix file\n");
}

int length, breadth;
char in_file[1024];
char out_file[1024];
float *m;
float mn, mx;

const int num_q = 1024;
int   *q_m;

void quantize(float *f, int s, int *q_m);
void findRange(float *m, int size, float & min, float & max);

int main(int argc, char **argv)
{
    if (argc <= n_args) { usage(); return 0; } 

    length = strtoul(argv[1], NULL, 10);
    breadth = strtoul(argv[2], NULL, 10);
    strcpy(in_file, argv[3]);
    strcpy(out_file, argv[4]);

    m = new float [length*breadth];
    for (int i=0;i <length*breadth;i++) m[i] = 0;
    q_m = new int [length*breadth];
    for (int i=0;i <length*breadth;i++) q_m[i] = 0;

    FILE * f_in = fopen(in_file, "r");
    FILE * f_out = fopen(out_file, "w");

    if (!f_in) { cout << "Failed to open input file " << in_file << endl; exit(0); }
    if (!f_out) { cout << "Failed to open output file " << out_file << endl; exit(0); }

    int i=0;
    float t_f;
    int t_i;
    while (!feof(f_in) && (i<length*breadth))
    {
	fscanf(f_in, "%f\n", &t_f);
        m[i++] = t_f;
    }

    findRange(m, length*breadth, mn, mx);
    cout << "Min: " << mn << endl;
    cout << "Max: " << mx << endl;

    quantize(m, length*breadth, q_m);

    fprintf(f_out, "%d\n", length);
    fprintf(f_out, "%d\n", breadth);
    for (i=0;i<length*breadth; i++) {
       fprintf(f_out, "%d\n", q_m[i]);
    }

    delete [] m;
    delete [] q_m;
    fclose(f_in);
    fclose(f_out);

    return 0;
}

void findRange(float *m, int s, float &min, float &max)
{
    min = 1000000;
    max = -1000000;
    for (int i=0; i<s; i++)
    {
        if (m[i] < min) min = m[i];
	    if (m[i] > max) max = m[i];
    }
}

void quantize(float *f, int s, int *q_m)
{
    float step = (mx - mn)/num_q;
    cout << "Step is " << step << endl;
    for (int i=0;i<s; i++)
    {
        float r = f[i]/step;
        if (r==num_q/2 || r == (num_q/2-1) || r == (num_q/2+1)) q_m[i++] = 0;
	    else q_m[i++] = (int) r+1;
    }    
}
