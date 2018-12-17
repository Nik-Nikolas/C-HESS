// C++HESS.
// (C)Igor Lobanov. 2017
//
// This is a header file.

#ifndef CPPHESSCLASSESBOARDG
#define CPPHESSCLASSESBOARDG

#include "C++HESS.h"

class BoardGlobals{
public:

  static const int32_t& getSize(){
    return size_;
  }

  static void setSize( const int32_t bsize ){
     size_ = bsize;
  }

  static const int32_t& getLongMoveStep(){
    return longMoveStep_;
  }

  static void setLongMoveStep( const int32_t longMoveStep ){
     longMoveStep_ = longMoveStep;
  }

  static const int32_t& getDelay(){
    return delay_;
  }

  static void setDelay( const int32_t delay ){
     delay_ = delay;
  }
private:
  static int32_t size_;
  static int32_t longMoveStep_;
  static int32_t delay_;
};
#endif