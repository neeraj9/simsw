/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef SLIDING_WN_H
#define SLIDING_WN_H  1

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include "CircularArray.h"
#include "consts.h"
#include "RandomNumber.h"

namespace SimSw {

  typedef unsigned int uint;

  struct probability_dist {
    float pcorl, pcorh;  // corrupt low & high limit
    float pdesl, pdesh;  // destroyed low & high limit

  };

  typedef struct probability_dist Pdistribution;

  class Sliding {
  public:
    static const int NOT_A_NUMBER = -100;
    static const int DUPLICATE_PKT = 100;
    static const int MAX_TIME_OUT = 4;
    static const int MAXWINSIZE = 200; // max value of max_num

    enum pktStatus { OK = 0, CORRUPT, DESTROYED, CANNOT_SEND };
    enum pktType { DATA = 0, ACK, NAK };

    static int is_true(float pl, float ph);
    static pktStatus getPktStatus(Pdistribution pd, float p);
    static int calcErrorDistribution(float pe, int n, Pdistribution& pdis);
    static int withinWindow(int start, int end, int num);
  };
}

#endif /* SLIDING_WN_H */
