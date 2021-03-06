/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dct.h"

// ********************************************************
int main() {
   short a[N_DCT], b[N_DCT], b_prime[N_DCT], x[10*N_DCT];
   int retval = 0, i, j, z;
   FILE *fp;

   fp=fopen("in.dat","r");
   //Read 640 entries from the File
   for (i=0; i<(10*N_DCT); i++){
      int tmp;
      fscanf(fp, "%d", &tmp);
      x[i] = tmp;
   }
   fclose(fp);


   //Call the DCT function 10 times for Dataflow
   for (i=0; i<10; i++){
	   printf("Checkpoint: i = %d\n", i);
	   // Iterate through the large x[] array to populate the smaller input array a[]
	   for (j=0; j<N_DCT; j++){
	      a[j] = x[j+(N_DCT*i)];
		   printf("Checkpoint: j = %d; ", j);
	   }//End j_loop
	   printf("\n");

	   //Call DCT for each iteration of a[]
	   dct(a, b);

	   //For testing purposes, save the first iteration of b[] to b_prime[]
	   //To compare to out.golden.dat
	   if (i==0){
		   printf("Copying array b to b_prime\n");
		   for (z=0; z<N_DCT; z++) {
				b_prime[z] = b[z];
		   }//End For
	   }//End IF
   }//End i_loop


	fp=fopen("out.dat","w");
	//printf(" Din Dout \n");
	for (i=0;i<N_DCT;i++) {
		fprintf(fp, "%d \n", b_prime[i]);
		//printf("  %d   %d\n", a[i], b[i]);
	}
	fclose(fp);

	// Compare the results file with the golden results
	retval = system("diff --brief -w out.dat out.golden.dat");
	if (retval != 0) {
		printf("Test failed  !!!\n"); 
		retval=1;
	} else {
		printf("Test passed !\n");
  }
   return retval;
}
