#include "Robot.h"
#include "Pins.h"

Robot* robot;
Bras* bras;

void setup() 
{
   Serial.begin(9600);
   
   bras  = new Bras(PIN_EPAULE, PIN_COUDE, PIN_POIGNET, PIN_MAIN);
   robot = new Robot(bras, PIN_SPEED, PIN_FORWARD, PIN_BACKWARD, PIN_SOUND0, PIN_SOUND1, PIN_BAR_SENSOR, PIN_LANCE_BALLE, PIN_DRAPEAU);
      
   pinMode(PIN_SWITCH, INPUT);
} 
  
void loop() 
{  
  while(digitalRead(PIN_SWITCH) != HIGH){robot->action(Arreter);}
  
  Serial.println("-----------------------------------------------------------------------------");
  
  //Compter les barres
  robot->evaluateBarSensorData(12);   
  

  Serial.print("\nPosition : "),Serial.println(robot->getPosition());
    
  int positionActuelle   = robot->getPosition();
  int positionNecessaire = robot->positionNeeded(robot->getEpreuve());
  
  if(positionActuelle < positionNecessaire)
  {
    robot->action(Avancer);
  }
  else if(positionActuelle > positionNecessaire)
  {
    robot->action(Reculer);
  }
  else  //Le robot est à la bonne place pour une épreuve
  {
    robot->action(Arreter);
  
    //Faire l'action appropriée
    switch(robot->getEpreuve())
    {
      case Debut:
        robot->debut();
        break;
      
      case PrendreBatterie:
        robot->prendreBatterie();
        break;
        
      case DetruireTour:
        robot->detruireTour();
        break;
      
      case LeverBarriere:
        robot->leverBarriere();
        break;
        
      case LancerPingPong:
        robot->lancerPingPong();
        break;
        
      case BarreMetal:
        robot->barreMetal();
        break;
        
      case DeposerBatterie:
        robot->deposerBatterie();
        break;
        
      case PrendreOeuf:
        robot->prendreOeuf();
        break;
        
      case BloquerLaser:
        robot->bloquerLaser();
        break;
        
      case ArreterMurphy:
        robot->arreterMurphy();
        break;    
      
      case Fin:
        robot->fin();
        delete robot;
        break;  
    }
  }  
}

