/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include "RandomNumber.h"

#include <cstdlib>

using namespace SimSw;

RandomNumber::RandomNumber() {
  rnum = 1; srand(rnum);
}

float
RandomNumber::get_rand() {
  float p;
  srand(rnum);
  rnum = rand();
  p = rnum/(float)RAND_MAX;

  return p;
}
