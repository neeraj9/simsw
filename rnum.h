/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

//     Working Correctly


#ifndef _RNUM_H
#define _RNUM_H 1

#include <stdlib.h>

class rNumber {
unsigned int rnum; // stores the current random value
public:
rNumber() {
  rnum = 1; srand(rnum);
}
float get_rand();   // returns number between 0 and 1
};

float rNumber::get_rand()
{
  float p;
  srand(rnum);
  rnum = rand();
  p = rnum/(float)RAND_MAX;

  return p;
}


#endif
