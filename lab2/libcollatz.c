//
// Created by defalt on 10.03.25.
//

#include "libcollatz.h"
int collatz_conjecture(int input){
  if( input % 2 == 0)
    return input / 2;
  else
    return 3*input +1;
 }

int test_collatz_convergence(int input, int *result, int max_iter) {
  for (int i = 0; i<max_iter; i++) {
    input = collatz_conjecture(input);
    result[i] = input;
    if (input == 1)
      return i+1;
  }
  return 0;
}