// const_def.h
// -----------
// Yves Masur 11/2014 (ymasur@microclub.ch)
// 26.07.2015 - cst pour index

#ifndef const_def_h
#define const_def_h

//version soft
#define VERSION 102

// taille buffer de char serial
#define MAX_SER 20

// definition des IO's
#define Led_Main (2)
#define LED_MAIN_ON() digitalWrite(Led_Main, HIGH)
#define LED_MAIN_OFF() digitalWrite(Led_Main, LOW)

// définition des angles des servos - doigts
// O: ouvert
// F: fermé
// D: droit

#define POUCE_O (10)
#define POUCE_F (125)
#define POUCE_D (75)
#define IDX_O (160)
#define IDX_F (0)
#define INDEX_O (0)
#define INDEX_F (175)
#define MAJEUR_O (0)
#define MAJEUR_F (170)
#define ANNULAIRE_O (0)
#define ANNULAIRE_F (160)

// définitions des angles A et hauteur H du bras
#define A_COU  15  // p5
#define H_COU  180

#define A_MENTON 15    // p4
#define H_MENTON 110

#define A_BOUCHE 75    // p3
#define H_BOUCHE 90

#define A_COTE  165  // p2
#define H_COTE  45

#define A_JOUE 100  // p1
#define H_JOUE 0

//timing pour mouvements

#define T_MVMT 350  // en ms
#define T_KEY_POS 3  // en 0.1 s
#define T_SERVO_ON 20 // en 0.1 s

//prototypes


#endif const_def_h
