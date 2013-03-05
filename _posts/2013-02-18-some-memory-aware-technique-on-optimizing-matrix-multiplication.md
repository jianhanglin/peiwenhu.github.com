---
layout: post
title: "Some memory aware technique on optimizing matrix multiplication"
description: "This is part of what I've been doing on experiments of matrix multiplication optimization."
category: Study
tags: memory matrix optimization
img: "/assets/MatrixMultiplication/mmm.jpg"
---
{% include JB/setup %}

###Idea courtesy:Computer Systems: a programmer's perspective. B&O

This is part of what I've been doing on experiments of matrix multiplication optimization. The basic idea is:  

* When doing a large amount of repeated work, like doing multiplication with large size of vectors, it is possible to reduce the 
identical instruction and loop overhead. For the former one, the Intel SSE intrinsics is used for SIMD(Single Instruction Multiple Data) operations. 
And for the latter one, techniques like loop unrolling and blocking are used. For MMM, since it's O(n\*n\*n) complexity, the effect of interchange the 
3 coordinates, `i`, `j`, and `k` is also worth considering.  

For the simplest implementation, a matrix multiplication function can be wrote as follows:  

	typedef float data_t;
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

Where c is the destination matrix and a and b are the two operands. By using a temporary variable `sum` it effectively reduces the number of accessing the 
same memory location of `c0[i*length+j]` for every inner loop. Instead, it only stores the value when the calculation is done for each element.  

One way to possibly increase its efficiency is to "unroll the loop", which means to reduce the loop overhead and typically this method often comes with 
multiple accumulators together to increase its parallelism.  
	
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
	
Unrolled 9 times with 9 accumulators.  

It is easy to notice that every calculation involves a row changing of matrix b. By interchange the inner loop, a better solution might be available:  

	
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

This time the granularity becomes a single element in the same row. Only `j` is changing in the inner loop, which utilizes the locality advantage of cache.  

Again, here's the unrolled&parallel version:  

	
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

To make it better, I used the SSE extension from Intel, specifically used for parellel operation on Intel CPU. It has the potential to increase the 
calculation by a maximum of 4 times. The concept of this guy is to store 4 data(2 if of double type) in a special `XMM` register thus execute one 
instruction on all the 4 data.  
	
	
	/* mmm ijk vector */
	/*
		typedef union {
		__m128 v;
		data_t d[4];
		} pack_t;
	*/
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

I transposed the matrix b so that it only has to multiply line-wise. A union `pact_t` is used to access the vector either in vector type or by single datum.  

Finally, here's the unrolled vectorized matrix multiplication. I could do better but that's really time-consuming.  

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

The result:  

![](/assets/MatrixMultiplication/mmm.jpg)  

[The complete code](/assets/MatrixMultiplication/mmm.c)  

[output data](/assets/MatrixMultiplication/mmm.csv)  