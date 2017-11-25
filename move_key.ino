// move_key.ino
// ------------
// Yves Masur 11/2014 (ymasur@microclub.ch)

#include "const_def.h"
#include "etats.h"

// main au repos
void Etats::init()
{
    key_repos();
    pos_repos();
}

// lecture des positions (7 moteurs)
void Etats::read_pos_doigts()
{
  p_pouce = pouce.read();
  p_idx_v = idx_v.read();
  p_index = index.read(); 
  p_majeur = majeur.read();
  p_annulaire = annulaire.read();
}

void Etats::read_pos_main()
{
  p_ecart = ecart.read();
  p_hauteur = elevation.read();
}

/*  pose_clef()
    -----------
    La fonction gère les mouvements des doigts, en prenant garde aux collisions.
    Les 5 servos des doigts reçoivent une position permettant une nouvellle clef LPC
    Entrée: 5 positions (0.. 180)
    Sortie: positions mémorisées
    Retour: -
*/
void Etats::pose_clef_d(int _pouce, int _idx_v, int _index, int _majeur, int _annulaire)
{
  int t = 0;  // tempo locale en cas de mouvements antagonistes
              // Note: la tempo entre clef est assumée par l'interpréteur
  
  LED_MAIN_ON();  // allume la LED mouvement en cours

// d'abord les doigts qui ouvrent, index change de position
  if (_index == INDEX_O && _index != p_index)
  {
    index.write(_index);
    t = tempos.mvmt;  // stocker une tempo
  }
  
  // d'abord les doigts qui ouvrent, majeur change de position
  if (_majeur == MAJEUR_O && _majeur != p_majeur)
  {
    majeur.write (_majeur);
    t = tempos.mvmt;  // stocker une tempo
  }
  
  delay(t);  // si demandé, délai suite à fermeture
  // le pouce peut collisioner en ouverture
  if (_pouce != POUCE_O && _pouce != p_pouce)
  {
    pouce.write(_pouce);
    annulaire.write(_annulaire);
    delay(tempos.mvmt);
    index.write(_index);
    majeur.write (_majeur); 
  }
  else
  {
    index.write(_index);      // en fermeture, avant le pouce
    if (_majeur != MAJEUR_F)  // seulement si le majeur ne se ferme pas
      majeur.write (_majeur); 
    delay(tempos.mvmt);
    pouce.write(_pouce);
    annulaire.write(_annulaire);   
  }
  
  majeur.write(_majeur);  // garantir ces mouvements, hors condition
  idx_v.write(_idx_v);
  delay(tempos.mvmt);
  LED_MAIN_OFF();  // eteint la LED mouvements en cours
  read_pos_doigts();        //relire les posiitons
} 

// ajoute un petit ecart après une clef
void Etats::ecart_fin()
{
  int depl;
  depl = etats.p_ecart + 15;
  ecart.write(depl); 
}
  
// initialisation: main au repos
void init_etats()
{
    etats.init();
}

// --- Les 8 clefs des doigts LPC ------------------------------
// repos, pouce droit, tout ouvert
void keyr()
{
  etats.pose_clef_d(POUCE_D,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_O);
}

// code 1 - p d j
void key1()
{
  etats.pose_clef_d(POUCE_D,IDX_F,INDEX_O,MAJEUR_F,ANNULAIRE_F);
}

// code 2 - k v z
void key2()
{
  etats.pose_clef_d(POUCE_D,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_F);
}

// code 3- sr
void key3()
{
  etats.pose_clef_d(POUCE_F,IDX_F,INDEX_F,MAJEUR_O,ANNULAIRE_O);
}

// code 4 - b n ui
void key4()
{
  etats.pose_clef_d(POUCE_F,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_O);
}

// code 5 - m t f
void key5()
{
  etats.pose_clef_d(POUCE_O,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_O);
}

// code 6 - l ch gn w
void key6()
{
  etats.pose_clef_d(POUCE_O,IDX_F,INDEX_O,MAJEUR_F,ANNULAIRE_F);
}

// code 7 - g
void key7()
{
  etats.pose_clef_d(POUCE_O,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_F);
}

// code 8 - y gn
void key8()
{
  etats.pose_clef_d(POUCE_F,IDX_O,INDEX_O,MAJEUR_O,ANNULAIRE_F);
}

// code espace - repos
void key_repos()
{
  etats.pose_clef_d(POUCE_D,IDX_F,INDEX_O,MAJEUR_O,ANNULAIRE_O);
}

// code doit d'honneur - I
void keyI()
{
  etats.pose_clef_d(POUCE_F,IDX_F,INDEX_F,MAJEUR_O,ANNULAIRE_F);
}
// --- fin des clefs -------------------------------------------

// --- Position ------------------------------------------------
// Positionne angle et hauteur
void pos_A_H(int angle, int hauteur)
{
  ecart.write(angle);
  elevation.write(hauteur);
  etats.read_pos_main();
}
  
// --- les 5 positions sur le visage
// position a - cou
void pos_a()
{
  pos_A_H(A_COU, H_COU);
}

// position b - menton
void pos_b()
{
  pos_A_H(A_MENTON, H_MENTON);
}
// position c - bouche
void pos_c()
{
  pos_A_H(A_BOUCHE, H_BOUCHE);
}
// position d - coté
void pos_d()
{
  pos_A_H(A_COTE, H_COTE);
}
// position e - joue
void pos_e()
{
  pos_A_H(A_JOUE, H_JOUE);
}
// position de repos
void pos_repos()
{
  pos_A_H(A_COTE, H_MENTON); 
}
// --- fin des positions ----------------------------

