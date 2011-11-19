#include<stdio.h>


int recIndexFloat( int i ) {

   if ( (i >= 4 && i <= 10)  ||  (i >= 13 && i <= 60) ||
        (i >= 61 && i <= 63)  ||  (i >= 65 && i <= 94) || 
        (i >= 98 && i <= 100)  ||  (i >= 103 && i <= 105) || 
        (i >= 108 && i <= 110)  ||  (i >= 113 && i <= 115) || 
        (i >= (61 + 55*1) && i <= (63 + 55*1))  ||  ( i >= (65 + 55*1) && i <= (94 + 55*1) ) || 
        (i >= (98 + 55*1) && i <= (100 + 55*1))  ||  (i >= (103 + 55*1) && i <= (105 + 55*1)) || 
        (i >= (108 + 55*1) && i <= (110 + 55*1))  ||  (i >= (113 + 55*1) && i <= (115 + 55*1)) || 
        (i >= (61 + 55*2) && i <= (63 + 55*2))  ||  ( i >= (65 + 55*2) && i <= (94 + 55*2) ) || 
        (i >= (98 + 55*2) && i <= (100 + 55*2))  ||  (i >= (103 + 55*2) && i <= (105 + 55*2)) || 
        (i >= (108 + 55*2) && i <= (110 + 55*2))  ||  (i >= (113 + 55*2) && i <= (115 + 55*2)) || 
        (i >= (61 + 55*3) && i <= (63 + 55*3))  ||  ( i >= (65 + 55*3) && i <= (94 + 55*3) ) || 
        (i >= (98 + 55*3) && i <= (100 + 55*3))  ||  (i >= (103 + 55*3) && i <= (105 + 55*3)) || 
        (i >= (108 + 55*3) && i <= (110 + 55*3))  ||  (i >= (113 + 55*3) && i <= (115 + 55*3)) || 
        (i >= (61 + 55*4) && i <= (63 + 55*4))  ||  ( i >= (65 + 55*4) && i <= (94 + 55*4) ) || 
        (i >= (98 + 55*4) && i <= (100 + 55*4))  ||  (i >= (103 + 55*4) && i <= (105 + 55*4)) || 
        (i >= (108 + 55*4) && i <= (110 + 55*4))  ||  (i >= (113 + 55*4) && i <= (115 + 55*4)) ) { 
      return 1;
   	}	
   else 
      return 0;
}


int recIndexFloatBetter( int i ) {

   if ( (i >= 4 && i <= 10)  ||  (i >= 13 && i <= 60) )
	return 1;

   int j;
   for (j=0; j<5; j++) {

        if ( (i >= (61 + 55*j) && i <= (63 + 55*j))   ||  
             (i >= (65 + 55*j) && i <= (94 + 55*j))   || 
             (i >= (98 + 55*j) && i <= (100 + 55*j))  ||  
	     (i >= (103 + 55*j) && i <= (105 + 55*j)) || 
             (i >= (108 + 55*j) && i <= (110 + 55*j)) ||  
 	     (i >= (113 + 55*j) && i <= (115 + 55*j))  ) {
      	   return 1;
   	}
   }
	
   return 0;

}



int main(int argc, char** argv) {

  int i = atoi(argv[1]);

  int ugly = recIndexFloat(i);
  int pretty = recIndexFloatBetter(i);

  printf("index from ugly function: %d     pretty function %d \n", 
	ugly, pretty);



  return 0;

}