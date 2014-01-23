#ifndef BRAS_H
#define BRAS_H 

#include "Servo.h"

//En cm
#define BRAS_LONGUEUR 22,5
#define AVANT_BRAS_LONGUEUR 29

#define MIN_PULSE_BATTERIE 1200
#define MIN_PULSE_OEUF 1400


/*
Documentation pour servomoteurs: http://arduino.cc/fr/Main/LibrairieServo
*/

enum ServoId
{
  Epaule,
  Coude,
  Poignet,
  Main,
};

enum Objet
{
  Batterie,
  Oeuf
};

enum MoveOrder
{
  ECP,
  EPC,
  CEP,
  CPE,
  PEC,
  PCE
};

class Bras
{
    public:
       Bras(const int pinEpaule, const int pinCoude, const int pinPoignet, const int pinMain);
       ~Bras();
      
      
       void syncMove(const float angleEpaule, const float angleCoude, const float anglePoignet, const MoveOrder order = ECP);
       void relativeSyncMove(const float relAngleEpaule, const float relAngleCoude, const float relAnglePoignet);
      
       void move(const double x, const double y);
      
       void fullExtension();
       void repos(const MoveOrder order = ECP);
       
       void saisir(const Objet objet);
       void relacher();
       
       void ranger();
       void ressortir(const Objet objet);
       
       static float radToDeg(float rad);
              
       float getAngle(const ServoId& id);
       void  setAngle(const ServoId& id, float angle);
     
    private:
       Servo* m_servoEpaule;
       Servo* m_servoCoude;
       Servo* m_servoPoignet;
       Servo* m_servoMain;
       Servo* getServoPtr(const ServoId& id); 
       
       ServoId* getMoveOrderTab(const MoveOrder order); 
       
       void secretSyncMove(const float angleEpaule, const float angleCoude, const float anglePoignet, const MoveOrder order = ECP);
};

#endif //ROBOT_H
