

/* 
 * LECTURE DES DONNES DU CODEUR INCREMENTALE : SENS et VITESSE
 */

// Debug Arduino communication
#define DEBUG         false

#if DEBUG
#define DEBUG_BEGIN(x);     Serial.begin(x);
#define DEBUG_PRINT(x);     Serial.print(x);
#define DEBUG_PRINTLN(x);   Serial.println(x);
#else
#define DEBUG_BEGIN(x);
#define DEBUG_PRINT(x);
#define DEBUG_PRINTLN(x);
#endif

// Motor
#define PWM_PIN       11
#define DIR_PIN       8
#define BRAKE_PIN     9 

// Codeur (pins d'interruptions)
#define CANAL_A_PIN   2
#define CANAL_B_PIN   3

#define RESOLUTION_C  500

// Directions moteurs
#define DIR_STOP      0
#define DIR_FORWARD   1
#define DIR_BACKWARD  2 
#define DIR_ERROR     3 

// Etats des canaux A et B du codeur

#define STATE_0       0
#define STATE_1       1
#define STATE_2       2
#define STATE_3       3

#define TIMEOUT       1


boolean dir_cmd   = true;          // Commande de Direction de rotation (true -> avant ; false -> arrière) du moteur
volatile int dir_read = 0;         // Commande de Direction de rotation du moteur

unsigned long time_counter_start;  // en [µs]
unsigned long time_counter_stop;   // en [µs]
unsigned long time_diff;           // en [µs]

unsigned long vitesse;             // en [tr/min]

volatile int state = 0;            // Etat actuel du codeur (voir machine à état)
volatile int next_state = 0;       // Etat suivant du codeur (voir machine à état)

volatile boolean imp_A = 0;        // Etat canal A du codeur
volatile boolean imp_B = 0;        // Etat canal B du codeur

volatile int compteurPeriode = 0;  // Compte jusqu'à TIMEOUT avant de faire le calcul de vitesse
volatile int stopvar = 0;

/*
 * Setup the pin outputs
 */
void setup()
{
  // Debug Arduino initialisation
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN("Demarrage d'une session DEBUG");
    
  // Motors Driver pins
  pinMode(DIR_PIN , OUTPUT);
  pinMode(PWM_PIN , OUTPUT);
  
  // Fréquence pwm à 31.3 kHz
  TCCR1B = TCCR1B & B11111000 | B00000001; 
  TCCR2B = TCCR2B & B11111000 | B00000001; 

  // Initialisation des entrées roue codeuse et de la routine d'interruption
  setup_roues_codeuses();
}

void loop()
{
  
  if (stopvar==1){
      analogWrite(PWM_PIN , 0);
    }
   else{
      analogWrite(PWM_PIN , 20);
    }
  digitalWrite(DIR_PIN , 1);

  /* test compteur
  time_counter_start = micros();
  delayMicroseconds(1000);
  time_counter_stop = micros();
  time_diff = time_counter_stop - time_counter_start;

  DEBUG_PRINT("Time : ");
  DEBUG_PRINTLN(time_diff);
  */
}

/*
 * Gestion des interruptions : 
 * 
 * INT0 correspond à la broche 2 des Arduino a base d’AVR et INT1 correspond à la broche 3. 
 * Pour accrocher une routine d’interruption à un état ou un changement d’état de l’une de ces 
 * broches on va utiliser la fonction attachInterrupt(...).
 * 
 * Cette fonction prend 3 arguments : 
 * - le numéro d’interruption externe, 
 * - la fonction à appeler quand l’interruption survient
 * - la condition selon laquelle l’interruption survient. 
 */
/*
 * Setup des informations des roues codeuses
 */
void setup_roues_codeuses()
{
    pinMode(CANAL_A_PIN , INPUT);
    pinMode(CANAL_B_PIN , INPUT);
    
    attachInterrupt(0, gestion_Codeur, CHANGE);  // Interrpution sur flanc montant/flanc descendant de CANAL_A_PIN
    attachInterrupt(1, gestion_Codeur, CHANGE);  // Interrpution sur flanc montant/flanc descendant de CANAL_B_PIN

    imp_A = digitalRead(CANAL_A_PIN);
    imp_B = digitalRead(CANAL_B_PIN);

    state = get_Encoder_State(imp_A, imp_B); // Etat initial de la sortie codeur
    time_counter_start = micros();
}



void gestion_Codeur()
{
  //if (compteurPeriode == TIMEOUT){
      //timeoutInterrupt();
      //compteurPeriode = 0;
    //}
  //else{compteurPeriode++;}
    // Lecture des canaux A et B du codeur
    imp_A = digitalRead(CANAL_A_PIN);
    imp_B = digitalRead(CANAL_B_PIN);
    //DEBUG_PRINT(imp_A);
    //DEBUG_PRINTLN(imp_B);

    next_state = get_Encoder_State(imp_A, imp_B);
    
    switch(state)
    {
      case STATE_0 : // 00
        switch(next_state)
        {
          case STATE_0 : dir_read = DIR_STOP;     vitesse = 0; break;
          case STATE_1 : dir_read = DIR_FORWARD;  break;
          case STATE_3 : dir_read = DIR_BACKWARD; break;
          case STATE_2 : Error1();                 break;
        }
        state = next_state;
        break;
      
      case STATE_1 : // 10
        switch(next_state)
        {
          case STATE_1 : dir_read = DIR_STOP;     vitesse = 0; break;
          case STATE_2 : dir_read = DIR_FORWARD;  break;
          case STATE_0 : dir_read = DIR_BACKWARD; break;
          case STATE_3 : Error1();                 break;
        }
        state = next_state;
        break;
      
      case STATE_2 : // 11
        switch(next_state)
        {
          case STATE_2 : dir_read = DIR_STOP;     vitesse = 0; break;
          case STATE_3 : dir_read = DIR_FORWARD;  break;
          case STATE_1 : dir_read = DIR_BACKWARD; break;
          case STATE_0 : Error1();                 break;
        }
        state = next_state;
        break;
      
      case STATE_3 : // 01
        switch(next_state)
        {
          case STATE_3 : dir_read = DIR_STOP;     vitesse = 0;  break;
          case STATE_0 : dir_read = DIR_FORWARD;  break;
          case STATE_2 : dir_read = DIR_BACKWARD; break;
          case STATE_1 : Error1();                 break;
        }
        state = next_state;
        break;

      default : 
        Error2();
        break;
    }
}


unsigned int get_Encoder_State(volatile boolean canal_A, volatile boolean canal_B)
{
  if (canal_A == false && canal_B == false)
  {
    return STATE_0;
  }
  else if (canal_A == true && canal_B == false)
  {
    return STATE_1;
  }
  else if (canal_A == true && canal_B == true)
  {
    return STATE_2;
  }
  else if (canal_A == false && canal_B == true)
  {
    return STATE_3;
  }
  else
  {
    Error3();
    return 0;
  }
}


unsigned long change_State_delay()
{
  unsigned long result;
  time_counter_stop = micros();
  result = time_counter_stop - time_counter_start;
  time_counter_start = time_counter_stop;
  return result;
}

void timeoutInterrupt(){
    time_diff = change_State_delay();
    vitesse = vitesse_Moteur(time_diff);
//    DEBUG_PRINT("Vitesse : ");
//    DEBUG_PRINTLN(vitesse);
//    DEBUG_PRINT("Time diff : ");
//    DEBUG_PRINTLN(time_diff);  
    DEBUG_PRINT("State : ");
    DEBUG_PRINTLN(state); 
}

unsigned long vitesse_Moteur(unsigned long timer)
{
  unsigned long result;
  result = (60000000)*((1/RESOLUTION_C)/timer);
  return result;
}


void Error1()
{   
    dir_read = DIR_ERROR;
    //DEBUG_PRINTLN("Passage d'etat impossible");
    stopvar = 1;
}

void Error2()
{   
    dir_read = DIR_ERROR;
    DEBUG_PRINTLN("Machine à états perdue");
}

void Error3()
{   
    dir_read = DIR_ERROR;
    DEBUG_PRINTLN("Etat inconnu");
}
