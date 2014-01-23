#ifndef ROBOT_H
#define ROBOT_H

#include "Sons.h"
#include "Servo.h"
#include "Bras.h"

enum MovementState
{
  Avancer,
  Reculer,
  Arreter
};

enum Epreuve
{
  Debut,
  PrendreBatterie,
  DetruireTour,
  LeverBarriere,
  LancerPingPong,
  BarreMetal,
  DeposerBatterie,
  PrendreOeuf,
  BloquerLaser,
  ArreterMurphy,
  Fin
};

class Robot
{
    public:
      Robot(Bras* bras, const int speedPin, const int avancerPin, const int reculerPin, const int soundPin0, const int soundPin1, const int barSensorPin, const int lanceBallePin, const int drapeauPin);
      ~Robot();
      
      MovementState getMovementState() ;
      void setMovementState(MovementState);
      
      void action(const MovementState& movementState);
      void setSpeed(const int vitesse);
      int getSpeed();
      
      int getPosition(); 
      int positionNeeded(const Epreuve epreuve);
     
      void evaluateBarSensorData(const float valeurLimite);
      
      float getDistFromSensor();
      
      void setMusicToPlay(const MovementState& state);
      
      void test();
      
      Epreuve getEpreuve();
      
      //Epreuves
      void debut();
      void prendreBatterie();
      void detruireTour();
      void leverBarriere();
      void lancerPingPong();
      void barreMetal();
      void deposerBatterie();
      void prendreOeuf();
      void bloquerLaser();
      void arreterMurphy();
      void fin();
      
    private:
      Bras *m_bras;
      Servo *m_servoLanceBalle, *m_servoDrapeau;
         
      void reculer();
      void avancer();
      void arreter();
    
      void ping();
      static float microsToCm(int ms);
      
      void updatePosition(); 
      
      Epreuve m_epreuve;
      
      MovementState m_movementState;
      int m_position;
      
      int m_motorSpeed;
      
      //Pins
      const int m_speedPin;
      const int m_avancerPin;
      const int m_reculerPin;
     
      const int m_soundPin0, m_soundPin1;
      
      const int m_barSensorPin;
      
      const int m_lanceBallePin;
      const int m_drapeauPin;  
        
};

#endif //ROBOT_H
