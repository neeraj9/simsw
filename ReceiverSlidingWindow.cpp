/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#include "ReceiverSlidingWindow.h"

using namespace std;
using namespace SimSw;

ReceiverSlidingWindow::ReceiverSlidingWindow(int psize,int maxn, int size) {

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
   ReceiverSlidingWindow::~ReceiverSlidingWindow()
   {
   if (buffer != NULL)
   delete[] buffer;
   }
 */

void
ReceiverSlidingWindow::setPara(int psize, int maxn, int maxw) {
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

void
ReceiverSlidingWindow::setErrorDistribution(float pe) {
  Sliding::calcErrorDistribution(pe,pktSize,pdis);
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
int
ReceiverSlidingWindow::send(int num, Sliding::pktType pt) {
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
      if (Sliding::withinWindow(sIndex,end_pkt,temp)) {
          while (Sliding::withinWindow(sIndex,end_pkt,temp)) {
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
int
ReceiverSlidingWindow::pktSend(Sliding::pktType pkt,Sliding::pktStatus& pktS) {
  int start_pkt;
  int num;
  float p;

  start_pkt = wArray.get_start();

  pktS = Sliding::CANNOT_SEND;
  switch (pkt) {
    case Sliding::ACK:      // what if the circarray is empty ??
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
          pktS = Sliding::getPktStatus(pdis,p);

/*			 if (num < 0)
        num += max_num; */                    // hey what if num = 1 but sIndex = 5 (say);
            // ie. ACK for 5,6,and 0 ; how to send that
            // solution (1): send ACK 7 and then send ACK 1
          send(num, Sliding::ACK); // solution (2): hope that on sending ACK 1 the sender will
          //  understand it (see on paper)
        }else
        return ERROR;
      break;
    default:  return ERROR;
    }

  return num;
}

int
ReceiverSlidingWindow::receive(int num,ofstream &fout) {

  int start_pkt, end_pkt;
  int i;
  int done, counter;


  start_pkt = wArray.get_start();
  end_pkt = wArray.get_end();

  if (wArray.is_empty())
    return Sliding::DUPLICATE_PKT;
  else if (num > max_num)
    return ERROR;
  else if (start_pkt > end_pkt) {
      if ( num < start_pkt && num > end_pkt)
        return Sliding::DUPLICATE_PKT;
    }else if ( num < start_pkt || num > end_pkt)
    return Sliding::DUPLICATE_PKT;

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
void
ReceiverSlidingWindow::getPktNumbers(char *s,int cmd,int start, int end) {
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

void
ReceiverSlidingWindow::getStrings(char *s1,char *s2, char *s3) {
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
ReceiverSlidingWindow::getPktNumbers(int *s,int cmd,int start, int end) {
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
ReceiverSlidingWindow::getArrays(int *s1,int *s2, int *s3) {
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
