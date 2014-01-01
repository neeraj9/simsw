/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef _CIRCULAR_ARRAY_H
#define _CIRCULAR_ARRAY_H 1

#include "consts.h"


class CircularArray {
int start,end;
  //	int *buffer;
int max_size;
public:
CircularArray(int size=0);
  // ~CircularArray() { if (max_size!=0) delete[] buffer; }
void set_size(int size, int s, int e);
int length(void);
int remove(int nos);
int insert(int nos);
int get_start(void) {
  return start;
}
int get_end(void)   {
  return end;
}
int is_empty(void);
};

CircularArray::CircularArray(int size)
{
// buffer = NULL;  no need to store anything
  start = end = -1;
  if (size<0)
    max_size = 0;
  else{
      max_size = size;
      //  buffer = new int[size];
    }

}


void CircularArray::set_size(int size, int s, int e)
{

  max_size = size;
  start = s;
  end = e;
}


int CircularArray::is_empty(void)
{
  return ( (start==-1) || (end == -1 ) );
}


int CircularArray::length(void)
{
  int len=0;

  if (is_empty())
    return 0;
    // else
  len = end - start + 1;
  if (len <= 0 )
    len = len + max_size;
  return len;
}

// can remove all the elmts.
int CircularArray::remove(int nos)
{
  if (is_empty() || (length() < nos) )
    return ERROR;
  if (length() == nos)
    start = end = -1;                                               // empty the array
  else
    start = (start + nos) % max_size;
  return DONE;
}

int CircularArray::insert(int nos)
{
  if (is_empty()) {
      if (nos > max_size)
        return ERROR;
        // else
      start = 0;
      end = nos-1;
      return DONE;
    }
  // else

  if (length()+nos > max_size)
    return ERROR;
    // else
  end = (end + nos) % max_size;
  return DONE;
}


#endif /* _CIRCULAR_ARRAY_H */
