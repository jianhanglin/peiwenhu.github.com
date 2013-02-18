/*****************************************************************************/
// gcc -O1 -o test_mmm_inter test_mmm_inter.c -lrt -lm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <xmmintrin.h>

#define GIG 1000000000
#define CPG 2.0           // Cycles per GHz -- Adjust to your computer

#define BASE  0
#define ITERS 20
#define DELTA 48
#define MAXSIZE BASE+ITERS*DELTA

#define INIT_LOW -10.0
#define INIT_HIGH 10.0
#define EPSILON (INIT_HIGH-INIT_LOW)*(double)(MAXSIZE)/2000000.0

#define OPTIONS 6    // TO BE MODIFIED
#define IDENT 0

typedef float data_t;

/* Create abstract data type for matrix */
typedef struct {
  long int len;
  data_t *data;
} matrix_rec, *matrix_ptr;

/* Number of bytes in a vector */
#define VBYTES 16

/* Number of elements in a vector */
#define VSIZE VBYTES/sizeof(data_t)

typedef data_t vec_t __attribute__ ((vector_size(VBYTES)));
typedef union {
  __m128 v;
  data_t d[4];
} pack_t;

/*****************************************************************************/
main(int argc, char *argv[])
{
  int OPTION;
  struct timespec diff(struct timespec start, struct timespec end);
  struct timespec time1, time2;
  struct timespec time_stamp[OPTIONS][ITERS+1];
  int clock_gettime(clockid_t clk_id, struct timespec *tp);
  matrix_ptr new_matrix(long int len);
  int set_matrix_length(matrix_ptr m, long int index);
  long int get_matrix_length(matrix_ptr m);
  int init_matrix(matrix_ptr m, long int len);
  int init_matrix_rand(matrix_ptr m, long int len);
  int zero_matrix(matrix_ptr m, long int len);
  void mmm_ijk(matrix_ptr a, matrix_ptr b, matrix_ptr c);
  void mmm_ijk9x(matrix_ptr a, matrix_ptr b, matrix_ptr c);
  void mmm_kij(matrix_ptr a, matrix_ptr b, matrix_ptr c);
  void mmm_kij8x(matrix_ptr a, matrix_ptr b, matrix_ptr c);
  void check_matrix(matrix_ptr a, matrix_ptr b);
  void mmm_ijkvec(matrix_ptr a, matrix_ptr b, matrix_ptr c,matrix_ptr z);
  void mmm_ijkvec5x(matrix_ptr a, matrix_ptr b, matrix_ptr c,matrix_ptr z);

  long int i, j, k;
  long int time_sec, time_ns;

  printf("\n Hello World -- MMM \n");

  // declare and initialize the matrix structure
  matrix_ptr a0 = new_matrix(MAXSIZE);
  init_matrix_rand(a0, MAXSIZE);
  matrix_ptr b0 = new_matrix(MAXSIZE);
  init_matrix_rand(b0, MAXSIZE);
  matrix_ptr c0 = new_matrix(MAXSIZE);
  zero_matrix(c0, MAXSIZE);
  matrix_ptr d0 = new_matrix(MAXSIZE);
  zero_matrix(d0, MAXSIZE);
  matrix_ptr z0 = new_matrix(MAXSIZE);
  zero_matrix(z0, MAXSIZE);  
  // create reference output matrix -- only biggest in sequence
  set_matrix_length(a0,MAXSIZE);
  set_matrix_length(b0,MAXSIZE);
  set_matrix_length(d0,MAXSIZE);

  mmm_ijk(a0,b0,d0);

  OPTION = 0;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
     mmm_ijk(a0,b0,c0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING ijk");   // only check last
  check_matrix(c0,d0);

  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    zero_matrix(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_ijk9x(a0,b0,c0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING ijk9x");
  check_matrix(c0,d0);

  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    zero_matrix(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_kij(a0,b0,c0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING kij");
  check_matrix(c0,d0);

  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    zero_matrix(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_kij8x(a0,b0,c0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING kij8x");
  check_matrix(c0,d0);

  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    set_matrix_length(z0,BASE+(i+1)*DELTA);
    zero_matrix(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_ijkvec(a0,b0,c0,z0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING kijvec");
    check_matrix(c0,d0);
  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(b0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    set_matrix_length(z0,BASE+(i+1)*DELTA);
    zero_matrix(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    mmm_ijkvec5x(a0,b0,c0,z0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    printf("\niter = %d", i);
  }
  printf("\nCHECKING kijvec6x");
    check_matrix(c0,d0);


  printf("\nlength, ijk, ijk9, kij, kij8, ijkvec,ijkvec6x");
  for (i = 0; i < ITERS; i++) {
    int elem=BASE+(i+1)*DELTA;
    printf("\n%d, ", elem);
    for (j = 0; j < OPTIONS; j++) {
      if (j != 0) printf(", ");
      printf("%f", ((double)(CPG)*(double)
		 (GIG * time_stamp[j][i].tv_sec + time_stamp[j][i].tv_nsec))/elem/elem/elem);
    }
  }

  printf("\n");
  
}/* end main */

/**********************************************/

/* Create matrix of specified length */
matrix_ptr new_matrix(long int len)
{
  long int i;

  /* Allocate and declare header structure */
  matrix_ptr result = (matrix_ptr) malloc(sizeof(matrix_rec));
  if (!result) return NULL;  /* Couldn't allocate storage */
  result->len = len;

  /* Allocate and declare array */
  if (len > 0) {
    data_t *data = (data_t *) _mm_malloc(len*len*sizeof(data_t),16);
    if (!data) {
	  free((void *) result);
	  printf("\n COULDN'T ALLOCATE STORAGE \n", result->len);
	  return NULL;  /* Couldn't allocate storage */
	}
	result->data = data;
  }
  else result->data = NULL;

  return result;
}

/* Set length of matrix */
int set_matrix_length(matrix_ptr m, long int index)
{
  m->len = index;
  return 1;
}

/* Return length of matrix */
long int get_matrix_length(matrix_ptr m)
{
  return m->len;
}

/* initialize matrix to i */
int init_matrix(matrix_ptr m, long int len)
{
  long int i;

  if (len > 0) {
    m->len = len;
    for (i = 0; i < len*len; i++)
      m->data[i] = (data_t)(i);
    return 1;
  }
  else return 0;
}

/* initialize matrix to rand */
int init_matrix_rand(matrix_ptr m, long int len)
{
  long int i;
  double fRand(double fMin, double fMax);

  if (len > 0) {
    m->len = len;
    for (i = 0; i < len*len; i++)
      m->data[i] = (data_t)(fRand((double)(INIT_LOW),(double)(INIT_HIGH)));
    return 1;
  }
  else return 0;
}

/* initialize matrix to zero */
int zero_matrix(matrix_ptr m, long int len)
{
  long int i,j;

  if (len > 0) {
    m->len = len;
    for (i = 0; i < len*len; i++)
      m->data[i] = (data_t)(IDENT);
    return 1;
  }
  else return 0;
}

data_t *get_matrix_start(matrix_ptr m)
{
  return m->data;
}

/* check matrix for differences */
void check_matrix(matrix_ptr c0, matrix_ptr d0)
{
  long int i, j;
  long int ERRORS = 0;
  long int length = get_matrix_length(c0);
  double MaxError = 0.0;
  double CurrentError;
  double fabs(double x);

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      //printf("\n%d %d %.0f %.0f",
      //	     i, j, c0->data[i*length+j], d0->data[i*length+j]);
      CurrentError = fabs((double)(c0->data[i*length+j]) -
			  (double)(d0->data[i*length+j]));
      if (CurrentError > (double)(EPSILON)) {
	ERRORS++;
        if (CurrentError > MaxError) MaxError = CurrentError;
	//printf("\ni = %d j = %d c0 = %.5f d0 = %.5f  ERROR = %.5f",
	//       i, j, c0->data[i*length+j], d0->data[i*length+j], CurrentError);
      }
    }
  if (ERRORS > 0)
    printf("\n%ld ERRORS!  Max Error = %f EPSILON = %f",
	   ERRORS, MaxError, EPSILON);
  else printf("\n OK!");
}

/*************************************************/

struct timespec diff(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}

double fRand(double fMin, double fMax)
{
  double f = (double)random() / (double)(RAND_MAX);
  return fMin + f * (fMax - fMin);
}

/*************************************************/

/* mmm ijk */
void mmm_ijk(matrix_ptr a, matrix_ptr b, matrix_ptr c)
{
  long int i, j, k;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t sum;

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      sum = 0;
      for (k = 0; k < length; k++)
	sum += a0[i*length+k] * b0[k*length+j];
      c0[i*length+j] = sum;
    }
}

/* mmm kij */
void mmm_kij(matrix_ptr a, matrix_ptr b, matrix_ptr c)
{
  long int i, j, k;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t r;

  for (k = 0; k < length; k++)
    for (i = 0; i < length; i++) {
      r = a0[i*length+k];
      for (j = 0; j < length; j++)
	c0[i*length+j] += r*b0[k*length+j];
    }
}

/* mmm ijk9x */
void mmm_ijk9x(matrix_ptr a, matrix_ptr b, matrix_ptr c)
{
  long int i, j, k;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t sum1;
  data_t sum2;
  data_t sum3;
  data_t sum4;
  data_t sum5;
  data_t sum6;
  data_t sum7;
  data_t sum8;
  data_t sum9;

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      sum1 = 0;
      sum2 = 0;
      sum3 = 0;
      sum4 = 0;
      sum5 = 0;
      sum6 = 0;
      sum7 = 0;
      sum8 = 0;
      sum9 = 0;

      for (k = 0; k < length-8; k+=9){
	sum1 += a0[i*length+k] * b0[k*length+j];
	sum2 += a0[i*length+k+1] * b0[(k+1)*length+j];
	sum3 += a0[i*length+k+2] * b0[(k+2)*length+j];
	sum4 += a0[i*length+k+3] * b0[(k+3)*length+j];
	sum5 += a0[i*length+k+4] * b0[(k+4)*length+j];
	sum6 += a0[i*length+k+5] * b0[(k+5)*length+j];
	sum7 += a0[i*length+k+6] * b0[(k+6)*length+j];
	sum8 += a0[i*length+k+7] * b0[(k+7)*length+j];
	sum9 += a0[i*length+k+8] * b0[(k+8)*length+j];

      }
      for (; k < length; k++){
	sum1 += a0[i*length+k] * b0[k*length+j];
      }
      c0[i*length+j] = sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+sum9;
    }
}

void mmm_kij8x(matrix_ptr a, matrix_ptr b, matrix_ptr c)
{
  long int i, j, k;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t r;

  for (k = 0; k < length; k++)
    for (i = 0; i < length; i++) {
      r = a0[i*length+k];
      for (j = 0; j < length-7; j+=8){
	c0[i*length+j] += r*b0[k*length+j];
	c0[i*length+j+1] += r*b0[k*length+j+1];
	c0[i*length+j+2] += r*b0[k*length+j+2];
	c0[i*length+j+3] += r*b0[k*length+j+3];
	c0[i*length+j+4] += r*b0[k*length+j+4];
	c0[i*length+j+5] += r*b0[k*length+j+5];
	c0[i*length+j+6] += r*b0[k*length+j+6];
	c0[i*length+j+7] += r*b0[k*length+j+7];
      }
      while(j<length){
	c0[i*length+j] += r*b0[k*length+j];
	j++;
      }
    }

}

/* mmm ijk vector */
void mmm_ijkvec(matrix_ptr a, matrix_ptr b, matrix_ptr c,matrix_ptr z)
{
  long int i=0, j=0, k=0,l;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t *z0 = get_matrix_start(z);//transposed
  __m128 sum;
  __m128 vA;
  __m128 vZ;
  
  __m128 temp;


  pack_t xfer;

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++)
      z0[j*length+i] = b0[i*length+j];
  //---
  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      sum = _mm_set_ps1(0);

      for (k = 0; k < length; k+=4){
	vA=_mm_load_ps(a0+i*length+k);
	vZ=_mm_load_ps(z0+j*length+k);

	temp = _mm_mul_ps(vA,vZ);
	sum = _mm_add_ps(sum,temp);
      }

      xfer.v=sum;
      c0[i*length+j] = xfer.d[0]+ xfer.d[1]+ xfer.d[2]+ xfer.d[3];

    }  
}
/* mmm ijk vector unroll 6x*/
void mmm_ijkvec5x(matrix_ptr a, matrix_ptr b, matrix_ptr c,matrix_ptr z)
{
  long int i=0, j=0, k=0,l;
  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
  long int length = get_matrix_length(a);
  data_t *a0 = get_matrix_start(a);
  data_t *b0 = get_matrix_start(b);
  data_t *c0 = get_matrix_start(c);
  data_t *z0 = get_matrix_start(z);//transposed
  __m128 sum1;
  __m128 sum2;
  __m128 sum3;
  __m128 sum4;
  __m128 sum5;
  __m128 sum6;
  __m128 vA;
  __m128 vZ;
  
  __m128 temp;


  pack_t xfer;

  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++)
      z0[j*length+i] = b0[i*length+j];
  //---
  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      sum1 = _mm_set_ps1(0);
      sum2 = _mm_set_ps1(0);
      sum3 = _mm_set_ps1(0);
      sum4 = _mm_set_ps1(0);
      sum5 = _mm_set_ps1(0);
      sum6 = _mm_set_ps1(0);

      for (k = 0; k < length-16; k+=20){
	vA=_mm_load_ps(a0+i*length+k);
	vZ=_mm_load_ps(z0+j*length+k);
	temp = _mm_mul_ps(vA,vZ);
	sum1 = _mm_add_ps(sum1,temp);

	vA=_mm_load_ps(a0+i*length+k+4);
	vZ=_mm_load_ps(z0+j*length+k+4);
	temp = _mm_mul_ps(vA,vZ);
	sum2 = _mm_add_ps(sum2,temp);


	vA=_mm_load_ps(a0+i*length+k+8);
	vZ=_mm_load_ps(z0+j*length+k+8);
	temp = _mm_mul_ps(vA,vZ);
	sum3 = _mm_add_ps(sum3,temp);


	vA=_mm_load_ps(a0+i*length+k+12);
	vZ=_mm_load_ps(z0+j*length+k+12);
	temp = _mm_mul_ps(vA,vZ);
	sum4 = _mm_add_ps(sum4,temp);


	vA=_mm_load_ps(a0+i*length+k+16);
	vZ=_mm_load_ps(z0+j*length+k+16);
	temp = _mm_mul_ps(vA,vZ);
	sum5 = _mm_add_ps(sum5,temp);

      }

      sum1=_mm_add_ps(sum1,sum2);
      sum3=_mm_add_ps(sum3,sum4);
      sum5=_mm_add_ps(sum5,sum6);

      sum1=_mm_add_ps(sum1,sum3);
      sum1=_mm_add_ps(sum1,sum5);      

      xfer.v=sum1;
      c0[i*length+j] = xfer.d[0]+ xfer.d[1]+ xfer.d[2]+ xfer.d[3];

    }  
}
