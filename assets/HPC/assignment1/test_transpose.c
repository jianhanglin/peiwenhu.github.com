/*****************************************************************************/
// gcc -O1 -o test_mmm_inter test_mmm_inter.c -lrt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define BASE  0
#define ITERS 300
#define DELTA 15

#define OPTIONS 3
#define IDENT 0
#define CYCLE_TIME 0.376
#define BLOCK_LENGTH 5
typedef double data_t;//8 byte

/* Create abstract data type for matrix */
typedef struct {
  long int len;
  data_t *data;
} matrix_rec, *matrix_ptr;

//struct timespec {
//  time_t tv_sec; /* seconds */
//  long tv_nsec;  /* nanoseconds */
//};

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
  int zero_matrix(matrix_ptr m, long int len);

  void block_transpose(matrix_ptr src, matrix_ptr des, int block_len);
  void transpose(matrix_ptr src, matrix_ptr des);
void transpose_rev(matrix_ptr src, matrix_ptr des);


//---##############################################################declaration complete
  long int i, j, k;
  long int time_sec, time_ns;
  long int MAXSIZE = BASE+(ITERS+1)*DELTA;

  //printf("\n Hello World -- MMM \n");

  // declare and initialize the matrix structure
  matrix_ptr a0 = new_matrix(MAXSIZE);
  init_matrix(a0, MAXSIZE);
  matrix_ptr c0 = new_matrix(MAXSIZE);
  zero_matrix(c0, MAXSIZE);

  OPTION = 0;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    	transpose( a0, c0);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    //  printf("\n1 iter = %d", i);
  }

  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    transpose_rev(a0,  c0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    //  printf("\n2 iter = %d", i);
  }
  
  OPTION++;
  for (i = 0; i < ITERS; i++) {
    set_matrix_length(a0,BASE+(i+1)*DELTA);
    set_matrix_length(c0,BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);

        block_transpose(a0,  c0, BLOCK_LENGTH);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
    //  printf("\n2 iter = %d", i);
  }
  

  for (i = 0; i < ITERS; i++) {
     long int loops; 
     loops=BASE+(i+1)*DELTA;
     printf("%ld,",loops);
    for (j = 0; j < OPTIONS; j++) {
      double cycle;
 
 
      cycle=(time_stamp[j][i].tv_sec*1000000000+time_stamp[j][i].tv_nsec)/CYCLE_TIME;
      printf("%f,", cycle/(loops*loops));
    }
    printf("\n");
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
    data_t *data = (data_t *) calloc(len*len, sizeof(data_t));
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

/* initialize matrix */
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

/* initialize matrix */
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

/*************************************************/

void block_transpose(matrix_ptr src, matrix_ptr des, int block_len)
{

  long int get_matrix_length(matrix_ptr m);

  data_t *get_matrix_start(matrix_ptr m);
//###########  
  long int length = get_matrix_length(src);
  
  if(length%block_len!=0){
    printf("not");
    return;
  }
  
  data_t *src_d = get_matrix_start(src);
  data_t *des_d = get_matrix_start(des);

  int i,j,k,l;
  
  for (i = 0; i < length; i += block_len) {
    for (j = 0; j < length; j += block_len) {
        for (k = i; k < i + block_len; k++) {
            for (l = j; l < j + block_len; l++) {
                des_d[k + l*length] = src_d[l + k*length];
            }
        }
     }
   } 
}
void transpose(matrix_ptr src, matrix_ptr des)
{

  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
//###########  
  long int length = get_matrix_length(src);
  
  
  data_t *src_d = get_matrix_start(src);
  data_t *des_d = get_matrix_start(des);

  int i,j;
  
  for (i=0; i < length; i++) {
            for (j = 0; j < length; j++) {
	      des_d[i + j*length] = src_d[j + i*length];
            }
        }
     
    
 
}

void transpose_rev(matrix_ptr src, matrix_ptr des)
{

  long int get_matrix_length(matrix_ptr m);
  data_t *get_matrix_start(matrix_ptr m);
//###########  
  long int length = get_matrix_length(src);
  
  
  data_t *src_d = get_matrix_start(src);
  data_t *des_d = get_matrix_start(des);

  int i,j;
  
  for (i=0; i < length; i++) {
            for (j = 0; j < length; j++) {
	      des_d[j + i*length] = src_d[i + j*length];
            }
        }
}
