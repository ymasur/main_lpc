// tempo.h
// -------
// Yves Masur 11/2014 (ymasur@microclub.ch)

#ifndef tempo_h
#define tempo_h

#include "const_def.h"

#define BOUND(val, min, max)       	\
	     {                          \
	     if (val<min) val=min;	\
	     if (val>max) val=max;	\
	     }


class Tempos
{
  public:
  Tempos();
  
  int mvmt;  // en ms
  int key;    // en 0.1 sec; clef et/ou position
  int pos;   // en 0.1 sec; clef et/ou position
  int servos_on;  // en .1 sec
};

#endif
 
