// etats.h
// -------
// Yves Masur 11/2014 (ymasur@microclub.ch)

#ifndef etats_h
#define etats_h

#include "const_def.h"

class Etats
{
  public:
  // positions: mémorise la dernière valeur appliquée (0..180) aux servos
  int p_pouce, p_idx_v, p_index, p_majeur, p_annulaire, p_ecart, p_hauteur;

  void init();      // main en position de repos
  void read_pos_doigts();  // relire les positions appliquées aux servos
  void read_pos_main();
  void pose_clef_d(int _pouce, int _idx_v, int _index, int _majeur, int _annulaire);
  void ecart_fin();
};

#endif
