/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

//     Working Correctly


#ifndef RANDOMNUMBER_H
#define RANDOMNUMBER_H 1

namespace SimSw {

  class RandomNumber {
    unsigned int rnum; // stores the current random value
  public:
    RandomNumber();
    float get_rand();   // returns number between 0 and 1
  };
}

#endif
