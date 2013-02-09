---
layout: post
title: "High performence computing:matrix multiplication"
description: ""
category: study
tags: [matrix multiplication]
---
{% include JB/setup %}


#High Performance Computing
##Matrix multiplication optimization

This report is focusing on the loop interchanging and blocking used in matrix multiplication optimization.  The source of experiments is based on the materials in Chapter 5 
of the book [_Computer Systems: A Programmer's Perspective_](http://www.amazon.com/Computer-Systems-Programmers-Perspective-2nd/dp/0136108040).  

#Part1: Testing code transformations -- Application: Combining data from 2D arrays

##1.Set Base to 0 and set DELTA and ITERS so that you test a wide range of array sizes. In particular

###How big can you make the array before you run into execution problems?

I can make it around 187MB, where base = 0, iters = 40, delta = 600. So it's 40\*600\*8 bytes.  
When I set it to be 218MB, there's a segmentation fault.  

###Which function is faster? Roughly by how much?

The normal one is faster. When the length is less than 700, the speed up is increasing from 0.7 to 4, and it stays around 
4 - 5 when the length is larger than 700. That means if the length is big enough, the normal one is 4 times faster than the reverse one.  

###At what array size(s) do the "interesting" things happen in the data? Why?

At about when the size is around 400. The time spent by the reverse one suddenly comes up and goes up sharply after that point.  
It's because the cache block size is less than the length of the data.  
When the cache block can no longer hold one line of data, or 1\*length , for the normal function, 
since it goes one element after another, it can still utilize the cache fully. But for the reverse one, it reads an element 
and then jumps to the element in the next line, which is out of the cache block when the line length becomes larger than 400.  
![Part1Q1](/assets/HPC/assignment1/Part1Prob1.jpg)
  
[This is the plot](/assets/HPC/assignment1/Part1Prob1.jpg)  
[This is the csv file](/assets/HPC/assignment1/csv/PART1/part1Prob1.csv)  

##2.Zoom in on two narrow ranges of array sizes, one small and one large (by adjusting BASE,ITERS,and DELTA).

###Use the methods you learned in Assignment 0 to find the number of cycles per element for those two ranges for the two different functions(four values in all)

![Part1Q2](/assets/HPC/assignment1/Part1Prob2.jpg)

[This is the plot](/assets/HPC/assignment1/Part1Prob2.jpg)  
[This is the csv file for large array](/assets/HPC/assignment1/csv/PART1/part1Prob2large.csv)  
[This is the csv file for small array](/assets/HPC/assignment1/csv/PART1/part1Prob2small.csv)  

The small array begins from 100 to 1000. The large array begins from 1110 to 1500. They all have ITERS = 40 and DELTA = 10.  

###Apply your knowledge of the computer interpret your observations here.

Array size-wise, the larger the array is, the more cycles it takes for both the normal function and the reverse function 
to compute. 

Cache-wise, in the large array, the effect of cache utilization is obvious, and results in the reverse function having more CPE 
than the normal one. But in small array whose length is smaller than a cache block, both functions can make use of cache, so the 
performances are almost identical.  

##3. You should find that the graphs of quantities (d) and (e) are particularly interesting. Zoom in on the region in (d) where it just becomes level and makes a transition.

###(d)should have a "u" shaped behavior. Explain why performance improves for a while. Explain why it gets worse.

I didn't find any "v" shape but it does goes low a little bit and then increses a little.  
It gets worse because the array size is increasing, out of the cache size.  

###(e)should have behavior that is more "jumpy". We will look at reasons why in a few weeks. But for now, do you have any qualitative ideas to explain this?

I investigated the values. In this table below shows the data at the jumping point.  

![Part1Q3table](/assets/HPC/assignment1/Part1Prob3table.tiff)

[Here is this image of table](/assets/HPC/assignment1/Part1Prob3table.tiff)

The E column is for normal function and the F is for the reversed. A is length.

When the length is less than 366, the CPE of normal function and that of the reverse function are almost the same. 
But when it is greater than 366 the CPE of reverse function jumps dramatically larger than the normal one. 

So that means the size of the cache(not sure which cache, maybe L3 or L2) that can hold one line of the array is around 366\*sizeof(data_t)==366\*8 bytes= 2.8 KB.  

#Part 2: Use these ideas on a more complex code -- Application: Optimize MMM using loop interchange.

##For each loop permutation(ijk, jki, and kij):

###1.How many plateaus are there?

	2.  

![whole log](/assets/HPC/assignment1/Part2Prob1wholeLog.jpg)

The log scale of the cycles per inner loop versus length of matrix  

![whole](/assets/HPC/assignment1/Part2Prob1whole.jpg)

The linear scale of the cycles per inner loop versus length of matrix  

[This is the csv file](/assets/HPC/assignment1/csv/PART2/whole.csv)  

###2.For each plateau, what is the number of cycles per iteration? Run new experiments to determine these, zooming in on points in the middle of the plateau.

For the the one at the beginning, the number of cycles per iteration is about 4.

![small](/assets/HPC/assignment1/Part2Prob2small.jpg)

For the stable one of MMM\_kij, the number of cpi is around 13.  
For the stable one of MMM\_ijk, the number of cpi is around 20.  
For the stable one of MMM\_jki, the number of cpi is around 30.  

###3.Where are the transitions? Again run new experiments, this time zooming in on the transitions.

The first one is around length=250, the second one is around length=370, and the third one at length = 800.  

![transition](/assets/HPC/assignment1/Part2Prob3.jpg)  
[This is the csv file](/assets/HPC/assignment1/csv/PART2/transition.csv)  

#Part 3: Use these ideas on a more complex code -- Application: Optimize MMM using blocking.

##1.benefit of using blocking

![block](/assets/HPC/assignment1/test_block.jpg)

The code is as below:

	void block_mmm_ijk(matrix_ptr a, matrix_ptr b, matrix_ptr c, int block_len)
	{
	  long int i, j, k;
	  long int kk,jj;
	  long int get_matrix_length(matrix_ptr m);
	
	  data_t *get_matrix_start(matrix_ptr m);
	  long int length = get_matrix_length(a);
	  if(length%block_len!=0){
		printf("not");
		return;
	  }
	  data_t *a0 = get_matrix_start(a);
	  data_t *b0 = get_matrix_start(b);
	  data_t *c0 = get_matrix_start(c);
	  data_t r;
	
	  double sum;
	  int en = block_len*(length/block_len);
	 
	 
	  for(kk=0;kk<en;kk+=block_len){
		for(jj=0;jj<en;jj+=block_len){
			for(i=0;i<length;i++){
				for(j=jj;j<jj+block_len;j++){
					sum=c0[i*length+j];
					for(k=kk;k<kk+block_len;k++){
						sum+=a0[i*length+k]*b0[k*length+j];
						c0[i*length+j] += sum;
						
					}
				
				}	
			}	
		}
	  } 

[The entire code is here](/assets/HPC/assignment1/test_mmm_block.c)  

So I set a Macro to be the length of the block, but still I pass it to this function.  
It has two block loops, for going through all the matrix with step `block_len`.  
Inside each block it does the ijk multiplication.  
The block_len has to divide the length.  
The blocking strategy has a constant performance, thus is always better than ijk and jki. But due to its overhead, 
the kij strategy outperforms it at the beginning, before the jump.  

##2.Optimal block size

Under the condition of my experiments, BASE = 0, ITERS = 20, DELTA = 50, I tested 4 block sizes,5,10,25,50.  
The stable lines in the figure are them, and among them the red line has the optimal size, which is 10.

![optimal](/assets/HPC/assignment1/optimalBlockSize.jpg)

[This figure can be viewed here](/assets/HPC/assignment1/optimalBlockSize.jpg)  
[This is the csv file](/assets/HPC/assignment1/csv/PART3/blockOpt50.csv)  
[This is the csv file](/assets/HPC/assignment1/csv/PART3/blockOpt25.csv)  
[This is the csv file](/assets/HPC/assignment1/csv/PART3/blockOpt10.csv)  
[This is the csv file](/assets/HPC/assignment1/csv/PART3/blockOpt5.csv)  


#Part 4:Use these ideas on an entirely new application: Matrix transpose.

##1.Implement the test_transpose.c

[Please see the code here](/assets/HPC/assignment1/test_transpose.c)

Normal function:


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

Blocking function:

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
	
##2&3.Examine the ij and ji loop permutations over a wide range of matrix sizes in Part1.

From the figure below it is perceivable that interchanging the i and j doesn't infect the performance. But the 
blocking does. I set the block size to be 5(the delta being 15 meanwhile).

![transpose](/assets/HPC/assignment1/Part4Prob2.jpg)
[This is the csv file](/assets/HPC/assignment1/csv/PART4/transpose1.csv)  
