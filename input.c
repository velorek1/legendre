#include <stdio.h>


// Branchless digit sum for numbers up to 9999
int sum_digits_linear(int x) {
    return x - ( ( (x - x%10)/10 + 
                   (x - x%100)/100 + 
                   (x - x%1000)/1000 ) * 9 );
}

int main() {
    int num=0;
    printf("Enter a number from [0-9999]: ");
    scanf("%d", &num);
    if (num>0 & num<10000){
      printf("The sum of all digits in your number amounts to: %d\n", sum_digits_linear(num));
    } else{
	    printf("Input error\n");
   }
}


