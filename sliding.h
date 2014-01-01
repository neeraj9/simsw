/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef SLIDING_WN_H
#define SLIDING_WN_H  1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include "circarr.h"
#include "consts.h"
#include "rnum.h"


typedef unsigned int uint;

#define NOT_A_NUMBER  -100
#define DUPLICATE_PKT 100
#define MAX_TIME_OUT  4
#define MAXWINSIZE  200 /* max value of max_num */

enum pktStatus { OK = 0, CORRUPT, DESTROYED, CANNOT_SEND };
enum pktType { DATA = 0, ACK, NAK };

char *stat_chr[]={"OK ","COR","DES","ERR"};

struct probability_dist {
  float pcorl, pcorh;  // corrupt low & high limit
  float pdesl, pdesh;  // destroyed low & high limit

};


typedef struct probability_dist Pdistribution;

int is_true(float pl, float ph);
pktStatus getPktStatus(Pdistribution pd, float p);
int calcErrorDistribution(float pe, int n, Pdistribution& pdis);
int withinWindow(int start, int end, int num);

// sender's
class sSlidingWindow {
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
circularArray wArray; // window array
rNumber rand_num;
int send(void); // send data

public:
sSlidingWindow(int psize=0,int maxn=1, int size=1);
void setPara(int psize, int maxn, int maxw,uint tout);
void setErrorDistribution(float pe);
int dataSend(pktStatus &pkt);
int receive(int num, pktType pt);
void getPktNumbers(char *s,int cmd=0,int start=0,int end=0);
void getPktNumbers(int *s,int cmd=0,int start=0, int end=0);
void getStrings(char *s1, char *s2, char *s3);
void getArrays(int *s1,int *s2, int *s3);
};



sSlidingWindow::sSlidingWindow(int psize, int maxn, int size)
{
  pktSize = psize;
  wSize = size;
  cSize = size;
  wArray.set_size(maxn,0,size-1);
  sIndex= 0; // pkt number start from zero
  max_num = maxn;

  pdis.pcorl = pdis.pcorh = 0.0;
  pdis.pdesl = pdis.pdesh = 0.0;
  timeOut = maxn + 1;
  time_counter = 0; // 0 ticks
  total_timeOuts = 0;
  retransmit_flag = FALSE;
  timeOut_flag = FALSE;
  pktNumber = 0;

  // when timeOut <= time_counter then retransmission starts


}
void sSlidingWindow::setPara(int psize, int maxn, int maxw,uint tout)
{
  pktSize = psize;
  wSize = maxw;
  cSize = maxw;
  wArray.set_size(maxn,0,maxw-1);
  sIndex= 0; // pkt number start from zero
  max_num = maxn;

  pdis.pcorl = pdis.pcorh = 0.0;
  pdis.pdesl = pdis.pdesh = 0.0;
  timeOut = tout;
  time_counter = 0; // 0 ticks
  total_timeOuts = 0;
  retransmit_flag = FALSE;
  timeOut_flag = FALSE;
  pktNumber = 0;

}


// this is not for retransmission
// it returns the packet number
int sSlidingWindow::send(void)
{
  int pktNum;


  pktNum = wArray.get_start();
  if (wArray.remove(1)!=ERROR) {
        // can send packet
      cSize--;

    }else
    return ERROR;

  return pktNum;
}



// num is the packet number
// if num<0 then it's not a number
// specifically if it is NOT_A_NUMBER
int sSlidingWindow::receive(int num, pktType pt)
{
  int slide=0; // slide by how much
  int temp;


  switch (pt) {
    case ACK:
      temp = num - 1;
      if (temp < 0 )
        temp += max_num;
      if (withinWindow(sIndex,wArray.get_end(),temp) == FALSE)
        return ERROR;
      // already received !!

      if (num >= 0) {

          if (num > (max_num - 1))
            return ERROR;
          // note that further error checking could be done !!

            // ACK num -- means that all pkts till (num-1) are acknowledged
          if (cSize == wSize ) // ie. window is full
            return ERROR;

          slide = num - sIndex;
          if (slide < 0)
            slide += max_num; // note: max_num may not be equal to wSize

        }else if (num == NOT_A_NUMBER)
        slide = 1;
      else
        return ERROR;
      /*	if (time_counter >= timeOut)
         time_counter = timeOut;
         else
       */
      time_counter = 0; // reset timer/retransmission
      total_timeOuts = 0;
      timeOut_flag = FALSE;
      retransmit_flag = FALSE;

        // Slide the starting index (not the left arm of the window !!!)
      sIndex = (sIndex + slide) % max_num;
        // Slide the right arm of the window
      wArray.insert(slide);
      cSize += slide;
      break;

    }

  return DONE;
}


int sSlidingWindow::dataSend(pktStatus &pkt)
{
  int pktNum;
  float p;
  int pnum;



    //	temp = time_counter - timeOut;
  if (total_timeOuts >= MAX_TIME_OUT)
    return CONNECTION_LOST;

  if (retransmit_flag == TRUE) {
      pktNumber = (pktNumber + 1) % max_num;
        // note: not to send all the pkts ; atleast 1 should remain
      if (pktNumber == wArray.get_end()) { // stop retransmit and start timer
          retransmit_flag = FALSE;
          timeOut_flag = TRUE;    // start the timer
          time_counter = 1;
          pkt = CANNOT_SEND;
          return ERROR;
        }
        // else
      pktNum = pktNumber;
    }else if (timeOut_flag == TRUE) {
      if (time_counter >= timeOut) { // Time out, now retransmit
          time_counter = 0;
          timeOut_flag = FALSE; // timer off; start retransmit
          total_timeOuts++;
          if (total_timeOuts >= MAX_TIME_OUT)
            return CONNECTION_LOST;

          retransmit_flag = TRUE;
          pktNumber = sIndex;
#ifdef debug
          cout << endl << "TIME OUT !!!!" << endl;
#endif
          pktNum = pktNumber;
        }else {
          time_counter++; // increment timer
          pkt = CANNOT_SEND;
          return ERROR;
        }
    }else {
      if (wArray.length() == 1) {
            // stop transmit - start timer
          retransmit_flag = FALSE; // not needed though !
          timeOut_flag = TRUE;  // start the timer
          time_counter = 1;
          pkt = CANNOT_SEND;
          cout << " ERROR ";
          return ERROR;
        }else
        pktNum = send();
    }

  p = rand_num.get_rand();
  pkt = getPktStatus(pdis,p);

  return pktNum;
}


void sSlidingWindow::setErrorDistribution(float pe)
{
  calcErrorDistribution(pe,pktSize,pdis);
#ifdef debug
  cout << " Sender's Distribution  ";
  cout << " Probabilities : - " << endl;
  cout << " Corrupted (low/high) = " << pdis.pcorl << " / ";
  cout << pdis.pcorh << endl;
  cout << " Destroyed (low/high) = " << pdis.pdesl << " / ";
  cout << pdis.pdesh << endl;
#endif

}

// returns the pack numbers in the string
// each numbers separated by '-'
// cmd is given by getStrings(), otherwise it takes default value
void sSlidingWindow::getPktNumbers(char *s,int cmd,int start, int end)
{
  int start_pkt;
  int end_pkt;
  int i;
  char temp[9];

  if (wArray.is_empty()) {
      if (cmd == 0)
        sprintf(s," Empty !! ");
      else
        s[0] = '\0';
      // return;
    }else {

      s[0] = '\0';
      if (cmd == 0) {
          start_pkt = wArray.get_start();
          end_pkt = wArray.get_end();
        }else {  // set externally
          start_pkt = start;
          end_pkt = end;
        }

      i = start_pkt;
      sprintf(temp,"%d-",i);
      strcat(s,temp);
      if (i != end_pkt) {
          i = (i + 1) % max_num;
          while (i != end_pkt) {
              sprintf(temp,"%d-",i);
              strcat(s,temp);
              i = (i + 1) % max_num;
            }
          sprintf(temp,"%d-",i);
          strcat(s,temp);
        }
    }
}

void sSlidingWindow::getStrings(char *s1,char *s2, char *s3)
{
  int start_pkt,start;
  int end_pkt,end;

  start_pkt = sIndex;
  end_pkt = ( sIndex + wSize - 1) % max_num;

  start = start_pkt;
  end = end_pkt;

  if (wArray.is_empty()) {
      getPktNumbers(s1,1,start,end);
      s2[0] = '\0';
    }else {
      if (sIndex == wArray.get_start()) {
          s1[0] = '\0';
        }else {   // if sIndex != wArray.get_start()
          end = wArray.get_start() - 1;
          if (end < 0)
            end += max_num;
          getPktNumbers(s1,1,start,end);
        }
      getPktNumbers(s2); // get what is in the window
    }
    // common to all
  start = (end_pkt + 1) % max_num;
  end = (start_pkt - 1);
  if (end < 0)
    end += max_num;
  getPktNumbers(s3,1,start,end);
}




// returns the pack numbers in the an array
// cmd is given by getStrings(), otherwise it takes default value
void sSlidingWindow::getPktNumbers(int *s,int cmd,int start, int end)
{
  int start_pkt;
  int end_pkt;
  int i;
  char temp[9];
  int counter=0;

  if (wArray.is_empty()) {
      if (cmd == 0)
        s[0] = -1; // to indicate end of array
      else
        s[0] = -1; // to indicate end of array
      // return;
    }else {

      if (cmd == 0) {
          start_pkt = wArray.get_start();
          end_pkt = wArray.get_end();
        }else {  // set externally
          start_pkt = start;
          end_pkt = end;
        }

      i = start_pkt;
      s[counter++] = i;
      if (i != end_pkt) {
          i = (i + 1) % max_num;
          while (i != end_pkt) {
              s[counter++] = i;
              i = (i + 1) % max_num;
            }
          s[counter++] = i;
        }
    }
  s[counter] = -1; // to indicate end of array
}

void sSlidingWindow::getArrays(int *s1,int *s2, int *s3)
{
  int start_pkt,start;
  int end_pkt,end;

  start_pkt = sIndex;
  end_pkt = ( sIndex + wSize - 1) % max_num;

  start = start_pkt;
  end = end_pkt;

  if (wArray.is_empty()) {
      getPktNumbers(s1,1,start,end);
      s2[0] = -1;  // -1 for end of array
    }else {
      if (sIndex == wArray.get_start()) {
          s1[0] = -1; // -1 for end of array
        }else {   // if sIndex != wArray.get_start()
          end = wArray.get_start() - 1;
          if (end < 0)
            end += max_num;
          getPktNumbers(s1,1,start,end);
        }
      getPktNumbers(s2); // get what is in the window
    }
    // common to all
  start = (end_pkt + 1) % max_num;
  end = (start_pkt - 1);
  if (end < 0)
    end += max_num;
  getPktNumbers(s3,1,start,end);
}



















// receiver's

class rSlidingWindow {
Pdistribution pdis;
int wSize; // Max Window size
int cSize; // Current Window size
int max_num; // max. number in the numbering scheme
int pktSize; // in bits
circularArray wArray; // window array
rNumber rand_num;
int sIndex;
int buffer[MAXWINSIZE]; // buffer for out of order packets
int send(int num, pktType pt);

public:
rSlidingWindow(int psize=0,int max_num=1, int size=1);
  //	~rSlidingWindow();
void setPara(int psize, int maxn, int maxw);
void setErrorDistribution(float pe);
int pktSend(pktType pkt,pktStatus &pktS);
int receive(int num,ofstream &fout); // data
void getPktNumbers(char *s,int cmd=0,int start=0,int end=0);
void getPktNumbers(int *s,int cmd=0,int start=0, int end=0);
void getStrings(char *s1, char *s2, char *s3);
void getArrays(int *s1,int *s2, int *s3);


};

rSlidingWindow::rSlidingWindow(int psize,int maxn, int size)
{

  pktSize = psize; // in bits
  max_num = maxn;
  wSize = size;
  cSize = size;
  sIndex = 0;
  wArray.set_size(maxn,0,size-1);
    //	buffer = NULL;
  pdis.pcorl = pdis.pcorh = 0.0;
  pdis.pdesl = pdis.pdesh = 0.0;

}


/*
   rSlidingWindow::~rSlidingWindow()
   {
   if (buffer != NULL)
   delete[] buffer;
   }
 */

void rSlidingWindow::setPara(int psize, int maxn, int maxw)
{
  int i;
  pktSize = psize;
  wSize = maxw;
  cSize = maxw;
  wArray.set_size(maxn,0,maxw-1);
  sIndex= 0; // pkt number start from zero
  max_num = maxn;
/*	if (buffer != NULL)
   delete[] buffer;

   buffer = new int[maxn];
 */
  for (i = 0; i < maxn; i++)
    buffer[i] = 0;                                            // reset buffer

  pdis.pcorl = pdis.pcorh = 0.0;
  pdis.pdesl = pdis.pdesh = 0.0;

}




void rSlidingWindow::setErrorDistribution(float pe)
{
  calcErrorDistribution(pe,pktSize,pdis);
#ifdef debug
  cout << " Receiver's Distribution  ";
  cout << " Probabilities : - " << endl;
  cout << " Corrupted (low/high) = " << pdis.pcorl << " / ";
  cout << pdis.pcorh << endl;
  cout << " Destroyed (low/high) = " << pdis.pdesl << " / ";
  cout << pdis.pdesh << endl;
#endif

}

// it is not for retransmission of ACK
// for sending ACK
// num is the ACK number
int rSlidingWindow::send(int num, pktType pt)
{
  int slide=0;
  int end_pkt;
  int temp;

  end_pkt = wArray.get_end();

  if (wArray.length() < wSize) {
        // can send ACK
        // assuming that ACK will be received without error
      if (wArray.is_empty()) { // should not happen
          return ERROR;
        }
      if (end_pkt == (max_num - 1)) { // ie. last possible# in the numbering
          slide = 0;      // scheme then don't slide the
          return num;   // window till that last # is received
        }    // Note: when the receiver will receive all the
             // packets in the receiver window then it'll
             // automatically expand the window
      slide = num - sIndex;
      if (slide < 0)
        slide += max_num; // note: max_num may not be equal to wSize


      temp = (end_pkt + slide) % max_num;
      if (withinWindow(sIndex,end_pkt,temp)) {
          while (withinWindow(sIndex,end_pkt,temp)) {
              slide--;
              if (slide <= 0 )
                break;
              temp = (end_pkt + slide) % max_num;
            }
        }
      if (slide > 0) {
          cSize += slide; // note that the right arm will move
          sIndex = (sIndex + slide) % max_num;
            // slide the window
          wArray.insert(slide);
        }
      // else don't slide the right arm
    }else // window full
    return ERROR;

  return num;
}

// first implementing only ACK
// it returns the ACK number to send.
int rSlidingWindow::pktSend(pktType pkt,pktStatus& pktS)
{
  int start_pkt;
  int num;
  float p;


  start_pkt = wArray.get_start();

  pktS = CANNOT_SEND;
  switch (pkt) {
    case ACK:      // what if the circarray is empty ??
      if (wArray.is_empty()) {
/*			 num = (sIndex + wSize - 1) % max_num; // last packet received
       num = (num + 1) % max_num; // new start or a new frame
       cSize = wSize;
       sIndex = num;
       wArray.set_size(max_num,sIndex,(sIndex + cSize - 1) % max_num);
       return num;
 */       return ERROR;
        }
        // else
      if (sIndex != start_pkt) { // ie. if can send ACK
          num = start_pkt;

          p = rand_num.get_rand();
          pktS = getPktStatus(pdis,p);

/*			 if (num < 0)
        num += max_num; */                    // hey what if num = 1 but sIndex = 5 (say);
            // ie. ACK for 5,6,and 0 ; how to send that
            // solution (1): send ACK 7 and then send ACK 1
          send(num,ACK); // solution (2): hope that on sending ACK 1 the sender will
          //  understand it (see on paper)
        }else
        return ERROR;
      break;
    default:  return ERROR;
    }

  return num;

}




int rSlidingWindow::receive(int num,ofstream &fout)
{

  int start_pkt, end_pkt;
  int i;
  int done, counter;


  start_pkt = wArray.get_start();
  end_pkt = wArray.get_end();

  if (wArray.is_empty())
    return DUPLICATE_PKT;
  else if (num > max_num)
    return ERROR;
  else if (start_pkt > end_pkt) {
      if ( num < start_pkt && num > end_pkt)
        return DUPLICATE_PKT;
    }else if ( num < start_pkt || num > end_pkt)
    return DUPLICATE_PKT;

  // else can receive the packet

  if (num != start_pkt) {
      buffer[num] = 1;   // got an out of order pkt
      fout << " num = " << num << " buffer[i] =" << buffer[num] << " ";
    }else {  // num == start_pkt
      buffer[num] = 1;
      done = TRUE;
      counter = 0;
      i = start_pkt;
      while ( (i != end_pkt) && done==TRUE) {
          counter++;
          buffer[i] = 0;  // reset the buffer as it goes along,
            // b'cause they'll be removed from the window
            // after the loop.
          i = (i + 1) % max_num;
          fout << " i = " << i <<" counter = "<< counter << " buffer[i] = " << buffer[i] << endl;
          if (buffer[i] == 0)
            done = FALSE;
        }
      if (done==TRUE) { // ie. if all the packets in the window have been received
#ifdef debug
          cout <<" complete window received!!!!  ";
#endif
          num = end_pkt; // needed for below instructions !!!!
          counter++;
          buffer[i] = 0;
        }
        // what if there is retransmission ???
      fout << " counter = " << counter <<"  ";
      wArray.remove(counter); // move the left arm by counter amount
        // initially buffer is all zero
      if (wArray.is_empty()) {
          if (num == (max_num -1) ) {    // ie. on receiving a complete frame
#ifdef debug
              cout << endl << " complete frame received!!!!  ";
#endif
              sIndex = max_num - 1;       // make a new window.
              cSize = wSize;
              wArray.set_size(max_num,0,cSize-2);  // ie. have a window with
              // max_num -1  pkt outside the
              // window.
              // max_num -1 is not acknowledged
            }else {
              cSize = (max_num-1) - num + 1;
              if (cSize > wSize)
                sIndex = num;
              else
                sIndex = (max_num - 1) - wSize + 1;
              cSize = wSize;
              wArray.set_size(max_num,num+1,sIndex + wSize - 1);
            }
        }

    }
  return DONE;
}


// returns the pack numbers in the string
// each numbers separated by '-'
void rSlidingWindow::getPktNumbers(char *s,int cmd,int start, int end)
{
  int start_pkt;
  int end_pkt;
  int i;
  char temp[9];

  if (wArray.is_empty()) {
      sprintf(s," Empty !! ");
      // return;
    }else {
      s[0] = '\0';
      if (cmd == 0) {
          start_pkt = wArray.get_start();
          end_pkt = wArray.get_end();
        }else {  // set externally
          start_pkt = start;
          end_pkt = end;
        }
      i = start_pkt;
      sprintf(temp,"%d-",i);
      strcat(s,temp);
      if (i != end_pkt) {
          i = (i + 1) % max_num;
          while (i != end_pkt) {
              sprintf(temp,"%d-",i);
              strcat(s,temp);
              i = (i + 1) % max_num;
            }
          sprintf(temp,"%d-",i);
          strcat(s,temp);
        }
    }
}

void rSlidingWindow::getStrings(char *s1,char *s2, char *s3)
{
  int start_pkt,start;
  int end_pkt,end;

  start_pkt = sIndex;
  end_pkt = ( sIndex + wSize - 1) % max_num;

  start = start_pkt;
  end = end_pkt;

  if (wArray.is_empty()) {
      getPktNumbers(s1,1,start,end);
      s2[0] = '\0';
    }else {
      if (sIndex == wArray.get_start()) {
          s1[0] = '\0';
        }else {   // if sIndex != wArray.get_start()
          end = wArray.get_start() - 1;
          if (end < 0)
            end += max_num;
          getPktNumbers(s1,1,start,end);
        }
      getPktNumbers(s2); // get what is in the window
    }
    // common to all
  start = (end_pkt + 1) % max_num;
  end = (start_pkt - 1);
  if (end < 0)
    end += max_num;
  getPktNumbers(s3,1,start,end);
}



// returns the pack numbers in the an array
// cmd is given by getStrings(), otherwise it takes default value
void rSlidingWindow::getPktNumbers(int *s,int cmd,int start, int end)
{
  int start_pkt;
  int end_pkt;
  int i;
  char temp[9];
  int counter=0;

  if (wArray.is_empty()) {
      if (cmd == 0)
        s[0] = -1; // to indicate end of array
      else
        s[0] = -1; // to indicate end of array
      // return;
    }else {

      if (cmd == 0) {
          start_pkt = wArray.get_start();
          end_pkt = wArray.get_end();
        }else {  // set externally
          start_pkt = start;
          end_pkt = end;
        }

      i = start_pkt;
      s[counter++] = i;
      if (i != end_pkt) {
          i = (i + 1) % max_num;
          while (i != end_pkt) {
              s[counter++] = i;
              i = (i + 1) % max_num;
            }
          s[counter++] = i;
        }
    }
  s[counter] = -1; // to indicate end of array
}

void rSlidingWindow::getArrays(int *s1,int *s2, int *s3)
{
  int start_pkt,start;
  int end_pkt,end;

  start_pkt = sIndex;
  end_pkt = ( sIndex + wSize - 1) % max_num;

  start = start_pkt;
  end = end_pkt;

  if (wArray.is_empty()) {
      getPktNumbers(s1,1,start,end);
      s2[0] = -1;  // -1 for end of array
    }else {
      if (sIndex == wArray.get_start()) {
          s1[0] = -1; // -1 for end of array
        }else {   // if sIndex != wArray.get_start()
          end = wArray.get_start() - 1;
          if (end < 0)
            end += max_num;
          getPktNumbers(s1,1,start,end);
        }
      getPktNumbers(s2); // get what is in the window
    }
    // common to all
  start = (end_pkt + 1) % max_num;
  end = (start_pkt - 1);
  if (end < 0)
    end += max_num;
  getPktNumbers(s3,1,start,end);
}

















/* this fn. returns true if the random no. is in the probability */
int is_true(float pl, float ph)
{
  float num=0.0;

  num = rand()/(float)RAND_MAX;
#ifdef debug
  cout << " num = " << num;
#endif
  if ( (num <= ph) && (num >= pl) )
    return TRUE;
  else
    return FALSE;

}


pktStatus getPktStatus(Pdistribution pd, float p)
{
  pktStatus pks;

//	p = rand()/(float)RAND_MAX;

  if ( (p <= pd.pcorh) && (p >= pd.pcorl) ) {
      if ( (p <= pd.pdesh) && (p >= pd.pdesl) )
        pks = DESTROYED;
      else
        pks = CORRUPT;
    }else // if OK
    pks = OK;

  return pks; // return the packet status
}

// pe == pkt error rate
// n == bits/packet
int calcErrorDistribution(float pe, int n, Pdistribution& pdis)
{
  long power;
  float pdestroyed;
  int x = 50; // 50% corrupted == destroyed


  pdis.pcorl = 0.00;
  pdis.pcorh = 1.0 - pow((1 - pe),n);

  power = n*x /100;
  pdestroyed = pow(pe,power) * pow((1-pe),n - power);

  pdis.pdesl = pdis.pcorh - pdestroyed;
  pdis.pdesh = pdis.pcorh;

  return DONE;
}

// tells if a number is within a window or not !
int withinWindow(int start, int end, int num)
{
  int result=FALSE;

  if (start < 0  || end < 0)
    result = ERROR;
  else if (start <= end) {
      if (num >= start && num <= end)
        result = TRUE;
    }else {  // start > end
      if (num >= start || num <= end)
        result = TRUE;
    }

  return result;
}



#endif /* SLIDING_WN_H */
