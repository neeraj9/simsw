/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef SENDERSLIDINGWINDOW_H
#define SENDERSLIDINGWINDOW_H  1

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include "CircularArray.h"
#include "consts.h"
#include "Sliding.h"

namespace SimSw {
  // sender's
  class SenderSlidingWindow {
    Pdistribution pdis;
    int wSize; // Window size (max)
    int cSize; // current window size
    int sIndex; // starting index ; note this pkt might me sent away
    int max_num; // max. number in the numbering scheme
    int pktSize; // in bits
    uint timeOut; // number of ticks after which
      // retransmission starts
    uint total_timeOuts; // number of consequtive time
      // out's
    uint time_counter; // counter for timeout
    int timeOut_flag;    // TRUE if time out
    int retransmit_flag; // TRUE if retransmiting
    int pktNumber;  // for keeping track of packet
      // number during retransmission.
    CircularArray wArray; // window array
    RandomNumber rand_num;
    int send(void); // send data

  public:
    SenderSlidingWindow(int psize=0,int maxn=1, int size=1);
    void setPara(int psize, int maxn, int maxw,uint tout);
    void setErrorDistribution(float pe);
    int dataSend(Sliding::pktStatus &pkt);
    int receive(int num, Sliding::pktType pt);
    void getPktNumbers(char *s,int cmd=0,int start=0,int end=0);
    void getPktNumbers(int *s,int cmd=0,int start=0, int end=0);
    void getStrings(char *s1, char *s2, char *s3);
    void getArrays(int *s1,int *s2, int *s3);
  };
}

#endif /* SENDERSLIDINGWINDOW_H */
