// tempo.ino
// ---------
// Yves Masur 11/2014 (ymasur@microclub.ch)

#include "tempo.h"

Tempos::Tempos()
{
  mvmt = T_MVMT;  // en ms
  key = T_KEY_POS; // en 0.1 sec
  pos = T_KEY_POS; // en 0.1 sec
  servos_on = T_SERVO_ON;  // en .1 sec
};
