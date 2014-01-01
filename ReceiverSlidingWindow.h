/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef RECEIVERSLIDINGWINDOW_H
#define RECEIVERSLIDINGWINDOW_H  1

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include "CircularArray.h"
#include "consts.h"
#include "sliding.h"

namespace SimSw {

  // receiver's
  class ReceiverSlidingWindow {
    Pdistribution pdis;
    int wSize; // Max Window size
    int cSize; // Current Window size
    int max_num; // max. number in the numbering scheme
    int pktSize; // in bits
    CircularArray wArray; // window array
    RandomNumber rand_num;
    int sIndex;
    int buffer[Sliding::MAXWINSIZE]; // buffer for out of order packets
    int send(int num, Sliding::pktType pt);

  public:
    ReceiverSlidingWindow(int psize=0,int max_num=1, int size=1);
      //	~ReceiverSlidingWindow();
    void setPara(int psize, int maxn, int maxw);
    void setErrorDistribution(float pe);
    int pktSend(Sliding::pktType pkt,Sliding::pktStatus &pktS);
    int receive(int num, std::ofstream &fout); // data
    void getPktNumbers(char *s,int cmd=0,int start=0,int end=0);
    void getPktNumbers(int *s,int cmd=0,int start=0, int end=0);
    void getStrings(char *s1, char *s2, char *s3);
    void getArrays(int *s1,int *s2, int *s3);
  };
}

#endif /* RECEIVERSLIDINGWINDOW_H */
