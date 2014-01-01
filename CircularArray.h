/**
 * SimSW: Simulate Sliding Window Protocol
 *
 * See LICENSE for license information.
 *
 * copyright (c) 2001 Neeraj Sharma <neeraj.sharma@alumni.iitg.ernet.in>
 */

#ifndef _CIRCULAR_ARRAY_H
#define _CIRCULAR_ARRAY_H 1

namespace SimSw {

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
    int get_start(void);
    int get_end(void);
    int is_empty(void);
  };
}

#endif /* _CIRCULAR_ARRAY_H */
