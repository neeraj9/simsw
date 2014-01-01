#include "Sliding.h"

using namespace SimSw;

const int Sliding::NOT_A_NUMBER;
const int Sliding::DUPLICATE_PKT;
const int Sliding::MAX_TIME_OUT;
const int Sliding::MAXWINSIZE;

/* this fn. returns true if the random no. is in the probability */
int
Sliding::is_true(float pl, float ph) {
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

Sliding::pktStatus
Sliding::getPktStatus(Pdistribution pd, float p) {
  Sliding::pktStatus pks;

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
int
Sliding::calcErrorDistribution(float pe, int n, Pdistribution& pdis) {
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
int
Sliding::withinWindow(int start, int end, int num) {
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
