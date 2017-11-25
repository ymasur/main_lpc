// main_lpc.ino
// ------------
// Yves Masur 03/2015 (ymasur@microclub.ch)
/*
  Programme qui active les servomoteurs de la main LPC.
  Dev: Yves Masur (ymasur@microclub.ch)
 */
#include <Servo.h>
#include "const_def.h"
#include "etats.h"
#include "tempo.h"

// Objets utilisés pour main LPC
Servo pouce, idx_v, index, majeur, annulaire;
Servo ecart, elevation;
Etats etats;
Tempos tempos;

const int LED13 = 13;
const unsigned long cycleTime = 100; // t boucle: .1 sec.
unsigned long previousMillis = 0UL;
int t_on;  // timer activité moteurs
int t_key;  // timer intercalaire clefs
int t_pos;  // timer intercalaire positions

char buffer[MAX_SER + 1];
int in_pos = 0; // cursor position
byte echo = true;
bool etat_LED13;


/* servo_ON()
   ----------
   Assigne les sorties aux servos moteurs. A partir de ce moment, un servo reçoit
   les impulsions de positionnement et peut faire un bruit
   Entrée: -
   Sortie: -
   Retour: -
*/
void servo_ON()
{
  // connexion correcte des servos
  pouce.attach(3);
  idx_v.attach(4);
  index.attach(5);
  majeur.attach(6);
  annulaire.attach(7);
  ecart.attach(8);
  elevation.attach(9);
}

/* servo_OFF()
   -----------
   Coupe les impulsions de sortie sur les servos. S'ils sont toujours alimentés, ils maintennent le couple
   de positionnement, mais n'essayent plus de corriger la position.
   Entrée: -
   Sortie: -
   Retour: -
*/
void servo_OFF()
{
  // déconnexion des servos de la main
  pouce.detach();
  idx_v.detach();
  index.detach();
  majeur.detach();
  annulaire.detach();
}

// initialisation
void setup()
{
  // les commandes sont données par le sériel
  Serial.begin(9600);

  // IO
  pinMode(Led_Main, OUTPUT);
  pinMode(LED13, OUTPUT);

  // connexion correcte des servos
  etats.init();
  servo_ON();
}

/* pulse_01Sec()
   -------------
   Tâches à 100 ms.
   Entrée: -
   Sortie: t_on, t_key, t_pos décrémentés si besoin
   Retour: -
*/
void pulse_01Sec()
{
  if (t_on)
  {
    t_on--;
    if (t_on == 0)
      servo_OFF();  // arrêt des moteurs
  }

  if (t_on == 2)
    etats.ecart_fin();

  if (t_key)
    t_key--;

  if (t_pos)
    t_pos--;
}

/*  set_clef(char c)
    ---------------
    Met en place la clef demandée selon son n°, par appel du positionnement
    des moteurs. La tempo intercalaire est chargée.
    Entrée: n° ascii de la clef
    Sortie: t_key, servos
    Retour: -
*/
void set_clef(char c)
{
  if (t_key)  // residu d'attente?
    delay(t_key * 100); //à effectuer

  t_key = tempos.key;  // recharger compteur

  //mode LPC, enclencher les moteurs
  t_on = tempos.servos_on;
  servo_ON();

  switch (c)
  {
    case '1':
      key1();
      break;
    case '2':
      key2();
      break;
    case '3':
      key3();
      break;
    case '4':
      key4();
      break;
    case '5':
      key5();
      break;
    case '6':
      key6();
      break;
    case '7':
      key7();
      break;
    case '8':
      key8();
      break;
    case 'i':
    case '9':
      keyI();
      break;
    case ' ':
    case '0':
      key_repos();
      break;
    case '\r':
    case '\n':
      break;
    default:
      Serial.print("ERROR C?\n");
  }// switch
}// set_clef()

/*  set_position(char c)
    --------------------
    Place le bras dans une des positions selon son n° par
    appel du positionnement des moteurs.
    La tempo intercalaire est chargée.
    Entrée: n° ascii de la position
    Sortie: t_pos, servos positionnés
    Retour: -
*/
void set_position(char c)
{
  if (t_pos)    // résidu d'attente?
    delay(t_pos * 100); // effectuer

  t_pos = tempos.pos; // charger compteur

  //mode LPC, enclencher les moteurs
  t_on = tempos.servos_on;
  servo_ON();

  switch (c)
  {
    case '5':
      pos_a();
      break;
    case '4':
      pos_b();
      break;
    case '3':
      pos_c();
      break;
    case '2':
      pos_d();
      break;
    case '1':
      pos_e();
      break;
    case '0':
      pos_repos();
      break;
    case '\r':
    case '\n':
      break;
    default:
      Serial.print("ERROR P?\n");
  }// switch
}// set_position()


/*  help()
    ------
    Aide principale
*/
void help()
{
  Serial.print("\nMain LPC - usage");
  Serial.print("\n----------------");
  Serial.print("\nFormat: c<n>p<n>");
  Serial.print("\nLes clefs (consonnes) sont positionées par c1 à c8.");
  Serial.print("\nLes positions de la main sont données par p1 à p5.");
  Serial.print("\n\nOn peut modifier les paramètres suivants.");
  Serial.print("\nv - indique les valeurs de tempo, on peut les modifier avec la syntaxe:");
  Serial.print("\n<var>=<val>");
  Serial.print("\nAvec: <var>=paramètre; <val>=valeur numérique en decimal");
  Serial.print("e - echo (1=on, 0=off) \n");
  Serial.print("s - tempo servo ON\n");
  Serial.print("t - tempo mouvements\n");
  Serial.print("r - retention (délai) entre clefs\n");
  Serial.print("\nLes valeurs par défaut sont appliquées au démarrage ou en cas\nd'erreur de saisie.\n");
  Serial.print("\nAutre commandes:\n");
  Serial.print("a - arret\n");
  Serial.print("o - moteurs OFF\n");
  Serial.print("O - moteurs ON\n");
  Serial.print("V - version\n");
}

/*  char * cmd(char *p)
    -------------------
    Interpreteur de commande. Le pointeur exécute et consomme la ligne de commande.
    On retourne la prochaine position à interpréter. L'interpréteur doit être appelé jusqu'à
    ce qu'on rencontre le \0 final de la chaine de commande.
    Les commandes on une lettre, et d'éventuels paramètres
    Entrée: pointeur sur la commande en ASCII
    Retour: pointeur sur la suite de la ligne de commande
*/
char * cmd(char *p)
{
  int tmp;  // var local pour conversion et limitation de valeur

  switch (*p)
  {
    case 0:		// pas de commande?
      return p;	// retour immédiat; pas de consommation de chr
      break;

      /* cmd 'a' - arrêt
      */
    case 'a':
      set_clef('4');
      set_position('5');
      delay(t_pos * 100); // effectuer tempo
      servo_OFF();
      ecart.detach();
      elevation.detach();
      Serial.write("\narret.\n");
      break;

      /* cmd 'c' - configuration
      */
    case 'c':
    case 'C':
      Serial.write(*p);
      p += 1;
      set_clef(*p);
      Serial.write(*p);
      break;

      /* cmd 'e' - echo
      */
    case 'e':
      p += 2;
      tmp = atoi(p);
      BOUND(tmp, 0, 1);
      echo = tmp;
      Serial.print("\ne(cho:"); Serial.print(echo);
      Serial.print("\n");
      break;

      /* cmd 'o' - moteurs off
      */
    case 'o':
      servo_OFF();
      ecart.detach();
      elevation.detach();
      Serial.write("\nmoteurs OFF\n");
      break;

      /* cmd 'O' - moteurs On
      */
    case 'O':
      servo_ON();
      Serial.write("\nmoteurs ON\n");
      break;

      /* cmd 'p' - position
         */
    case 'p':
    case 'P':
      Serial.write(*p);
      p += 1;
      set_position(*p);
      Serial.write(*p);
      break;

      /* cmd 'r' - retention (tempo) après clef
      */
    case 'r':
      p += 2;
      tmp = atoi(p);
      while (isDigit(*p)) p++; // avance pointeur
      if (tmp == 0)
        tmp = T_KEY_POS;  // val. par défaut
      BOUND(tmp, 1, 60);
      tempos.key = tempos.pos = tmp;
      Serial.print("\nr(etention apres clef [1/10s]: "); Serial.print(tempos.key);
      Serial.print("\n");
      break;

      /* cmd 's' - servo On, durée
      */
    case 's':
      p += 2;
      tmp = atoi(p);
      while (isDigit(*p)) p++; // avance pointeur
      if (tmp == 0)
        tmp = T_SERVO_ON;  // val. par défaut
      BOUND(tmp, 3, 50);
      tempos.servos_on = tmp;
      Serial.print("\ns(ervo enclenchés [1/10s]: "); Serial.print(tempos.servos_on);
      Serial.print("\n");
      break;

      /* cmd 't' - tempo entre clef
      */
    case 't':
      p += 2;
      tmp = atoi(p);
      while (isDigit(*p)) p++; // avance pointeur
      if (tmp == 0)
        tmp = T_MVMT;  // val. par défaut
      BOUND(tmp, 150, 2000);
      tempos.mvmt = tmp;
      Serial.print("\nt(empo mouvement [ms]: "); Serial.print(tempos.mvmt);
      Serial.print("\n");
      break;

      /* cmd 'v' - voir les valeurs stockées
      */
    case 'v':
      Serial.print("\nTempos actuelles: ");
      Serial.print("\nt(empo mouvement [ms]: "); Serial.print(tempos.mvmt);
      Serial.print("\nr(etention apres clefs [1/10s]: "); Serial.print(tempos.key);
      Serial.print("\ns(ervo enclenchés [1/10s]: "); Serial.print(tempos.servos_on);
      Serial.print("\ne(cho [0-1]:"); Serial.print(echo);
      Serial.print("\n");
      break;

      /* cmd 'V' - Version
      */
    case 'V':
      Serial.print(VERSION);
      break;

      /* Aide
       */
    case '?':
      help();
      break;

      /* blancs
      */
    case ' ':
    case '\t':
    case '\r':
    case '\n':
      break;

    default:
      Serial.print(*p);
      Serial.print(" - ERROR cmd inconnue\n");
      break;
  }
  p += 1; // prochaine commande
  return p;
}// cmd()

/*  interpreter()
    -------------
    Lit les chr série un à un; les stocke. Dès que CR/LF est reconnu,
    apelle l'interpréteur de commande.
    E: -
 */
void interpreter()
{
  if (Serial.available() > 0) 	// Q: chr lu?
  {
    int inByte = Serial.read();
    { // R: oui,
      buffer[in_pos++] = inByte;  //stocker la ligne de commande
      if (in_pos == MAX_SER)      //gérer débordement
      {
        in_pos = 0;
        Serial.print("?\n");
      }
      else
      {
        if (echo)
          Serial.write(inByte);      //echo
      }

      if (inByte == '\r' || inByte == '\n') // Q:ligne entrée?
      { // R: oui, exécuter la(les) commande(s)
        char * q = buffer;
        while (*q)          // tant qu'il y en a, éxécuter
          q = cmd(q);

        q = buffer + MAX_SER + 1; //clean buffer
        while (q >= buffer)
          *q-- = 0;

        in_pos = 0;        // buffer prêt pour commande suivante
      }//cr/lf
    }//serial read
  }//if serial
}

/* Boucle principale.
   Tourne à 100 ms, commute la LED13 et apelle pulse_01Sec()
   apelle l'interpréteur en continu
*/
void loop()
{
  unsigned long currentMillis = millis();  // t écoulé
  if (currentMillis - previousMillis > cycleTime)
  {
    previousMillis = currentMillis;
    pulse_01Sec();
    etat_LED13 = !etat_LED13;
    digitalWrite(LED13, etat_LED13);
  }

  interpreter();

}//loop

