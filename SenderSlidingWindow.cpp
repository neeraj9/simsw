/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include "SenderSlidingWindow.h"

using namespace std;
using namespace SimSw;

SenderSlidingWindow::SenderSlidingWindow(int psize, int maxn, int size) {
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
void
SenderSlidingWindow::setPara(int psize, int maxn, int maxw,uint tout) {
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
int
SenderSlidingWindow::send() {
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
int
SenderSlidingWindow::receive(int num, Sliding::pktType pt) {
  int slide=0; // slide by how much
  int temp;

  switch (pt) {
    case Sliding::ACK:
      temp = num - 1;
      if (temp < 0 )
        temp += max_num;
      if (Sliding::withinWindow(sIndex,wArray.get_end(),temp) == FALSE)
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

        }else if (num == Sliding::NOT_A_NUMBER)
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

int
SenderSlidingWindow::dataSend(Sliding::pktStatus &pkt) {
  int pktNum;
  float p;
  int pnum;

    //	temp = time_counter - timeOut;
  if (total_timeOuts >= Sliding::MAX_TIME_OUT)
    return CONNECTION_LOST;

  if (retransmit_flag == TRUE) {
      pktNumber = (pktNumber + 1) % max_num;
        // note: not to send all the pkts ; atleast 1 should remain
      if (pktNumber == wArray.get_end()) { // stop retransmit and start timer
          retransmit_flag = FALSE;
          timeOut_flag = TRUE;    // start the timer
          time_counter = 1;
          pkt = Sliding::CANNOT_SEND;
          return ERROR;
        }
        // else
      pktNum = pktNumber;
    }else if (timeOut_flag == TRUE) {
      if (time_counter >= timeOut) { // Time out, now retransmit
          time_counter = 0;
          timeOut_flag = FALSE; // timer off; start retransmit
          total_timeOuts++;
          if (total_timeOuts >= Sliding::MAX_TIME_OUT)
            return CONNECTION_LOST;

          retransmit_flag = TRUE;
          pktNumber = sIndex;
#ifdef debug
          cout << endl << "TIME OUT !!!!" << endl;
#endif
          pktNum = pktNumber;
        }else {
          time_counter++; // increment timer
          pkt = Sliding::CANNOT_SEND;
          return ERROR;
        }
    }else {
      if (wArray.length() == 1) {
            // stop transmit - start timer
          retransmit_flag = FALSE; // not needed though !
          timeOut_flag = TRUE;  // start the timer
          time_counter = 1;
          pkt = Sliding::CANNOT_SEND;
          cout << " ERROR ";
          return ERROR;
        }else
        pktNum = send();
    }

  p = rand_num.get_rand();
  pkt = Sliding::getPktStatus(pdis,p);

  return pktNum;
}

void
SenderSlidingWindow::setErrorDistribution(float pe) {
  Sliding::calcErrorDistribution(pe,pktSize,pdis);
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
void
SenderSlidingWindow::getPktNumbers(char *s,int cmd,int start, int end) {
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

void
SenderSlidingWindow::getStrings(char *s1,char *s2, char *s3) {
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
void
SenderSlidingWindow::getPktNumbers(int *s,int cmd,int start, int end) {
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

void
SenderSlidingWindow::getArrays(int *s1,int *s2, int *s3) {
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
