#include "Bras.h"

Bras::Bras(const int pinEpaule, const int pinCoude, const int pinPoignet, const int pinMain)
{
   m_servoEpaule  = new Servo();
   m_servoCoude   = new Servo();
   m_servoPoignet = new Servo();
   m_servoMain    = new Servo();
  
   m_servoEpaule  ->attach(pinEpaule,  550, 2400);  // Sticker arraché: tester pour savoir si les valeurs min/max sont bonnes
   m_servoCoude   ->attach(pinCoude,   900, 2400);
   m_servoPoignet ->attach(pinPoignet, 550, 2400);  
   m_servoMain    ->attach(pinMain,    630, 2330); 
}

Bras::~Bras()
{
   m_servoEpaule ->detach();
   m_servoCoude  ->detach();
   m_servoPoignet->detach();
   m_servoMain   ->detach();
   
   delete m_servoEpaule;
   delete m_servoCoude;
   delete m_servoPoignet;
   delete m_servoMain;
}

Servo* Bras::getServoPtr(const ServoId& id)
{
  switch(id)
  {
    case Epaule:
      return m_servoEpaule;
    case Coude:
      return m_servoCoude;
    case Poignet:
      return m_servoPoignet;
    case Main:
      return m_servoMain;
  }
}

float Bras::radToDeg(float rad)
{
  while(rad > 2*M_PI)
    rad -= 2*M_PI;
  
  
  return rad/(2*M_PI)*360;
}

//Setters
void Bras::setAngle(const ServoId& id, float angle)
{
  Servo* servo = getServoPtr(id);
  
  float correction, minAngle, maxAngle;
  
  switch(id)
  {
    case Epaule:
      minAngle = 0;
      maxAngle = 180;
      break;
    case Coude:
      minAngle = 0;
      maxAngle = 180;
      break;
    case Poignet:
      angle = 200-angle;
      minAngle = 0;
      maxAngle = 160;
      break;
    case Main:
      minAngle = 60;
      maxAngle = 140;
      break;
  } 

  
  if(angle<minAngle)
    angle=minAngle;
  else if(angle>maxAngle)
    angle=maxAngle;
  
  //Déplacement
  float delta = angle - getAngle(id);
  
  if(delta > 0)
  {
    for(float i=getAngle(id);  i<angle; i+=1)
    {
      servo->write(i);
      delay(20);
    }
  }
  else if (delta < 0)
  {
    for(float i=getAngle(id);  i>angle; i-=1)
    {
      servo->write(i);
      delay(20);
    }
  }
}

//Getters
float Bras::getAngle(const ServoId& id)
{
  Servo* servo = getServoPtr(id);
  float angle = servo->read();
  
  return angle;
}

void Bras::syncMove(const float angleEpaule, const float angleCoude, const float anglePoignet, const MoveOrder order)
{
  float angle[3];
  
  switch(order)
  {
    case ECP: angle[0] = angleEpaule;  angle[1] = angleCoude;   angle[2] = anglePoignet; break;
    case EPC: angle[0] = angleEpaule;  angle[1] = anglePoignet; angle[2] = angleCoude; break;
    case CEP: angle[0] = angleCoude;   angle[1] = angleEpaule;  angle[2] = anglePoignet; break;
    case CPE: angle[0] = angleCoude;   angle[1] = anglePoignet; angle[2] = angleEpaule; break;
    case PEC: angle[0] = anglePoignet; angle[1] = angleEpaule;  angle[2] = angleCoude; break;
    case PCE: angle[0] = anglePoignet; angle[1] = angleCoude;   angle[2] = angleEpaule; break;
  }
  
  secretSyncMove(angle[0] ,angle[1], angle[2], order); 
}


void Bras::secretSyncMove(const float angleEpaule, const float angleCoude, const float anglePoignet, const MoveOrder order)
{
  ServoId id[3];
  for(int i=0; i<3;i++)
  {
    id[i] = getMoveOrderTab(order)[i];
  } 
  
  float currentAngle[3];
  for(int i=0; i<3; i++)
  {
    currentAngle[i] = getAngle(id[i]);  
  }
  
  currentAngle[3] = 200 - currentAngle[3];
    
  float finalAngle[3] = 
  {
    angleEpaule, 
    angleCoude, 
    anglePoignet
  };
  
  float deltaAngle[3];
  for(int i=0; i<3; i++)
  {
    deltaAngle[i] = currentAngle[i] - finalAngle[i];
  }
  
  float referenceAngle = 0; // Reference pour les vitesses de déplacement
  for(int i=0; i<3; i++)
  {
    referenceAngle = max(referenceAngle, abs(deltaAngle[i]));
  }
  
  float angleFraction[3];
  for(int i=0; i<3; i++)
  {
    angleFraction[i] = deltaAngle[i] / referenceAngle;
  }
  
  //Déplacement synchronisé des servomoteurs
//  for(int i=0; i<referenceAngle; i++)
//  {
//    for(int i=0; i<3; i++)
//    {
//      setAngle(id[i],   getAngle(id[i]) + angleFraction[i]);      
//    }
//  }

  //ServoId* tab = getMoveOrderTab(order);
  
  //Pour s'assurer que les servos sont aux bons angles à la fin
  for(int i=0; i<3; i++)
  {
    setAngle(id[i],  finalAngle[i]);
  }
}

ServoId* Bras::getMoveOrderTab(const MoveOrder order)
{
  ServoId tab[3];
  switch(order)
  {
    case ECP: tab[0] = Epaule;  tab[1] = Coude;   tab[2] = Poignet; break;
    case EPC: tab[0] = Epaule;  tab[1] = Poignet; tab[2] = Coude; break;
    case CEP: tab[0] = Coude;   tab[1] = Epaule;  tab[2] = Poignet; break;
    case CPE: tab[0] = Coude;   tab[1] = Poignet; tab[2] = Epaule; break;
    case PEC: tab[0] = Poignet; tab[1] = Epaule;  tab[2] = Coude; break;
    case PCE: tab[0] = Poignet; tab[1] = Coude;   tab[2] = Epaule; break;
  }
  
  for(int i=0; i<3; i++)
  {
    tab[i];
    Serial.println(tab[i]);  //WTF? I know right
  }
  
  return tab;
}

void Bras::relativeSyncMove(const float relAngleEpaule, const float relAngleCoude, const float relAnglePoignet)
{
  float angleEpaule  = getAngle(Epaule)  + relAngleEpaule;
  float angleCoude   = getAngle(Coude)   + relAngleCoude;
  float anglePoignet = getAngle(Poignet) + relAnglePoignet;
  
  syncMove(angleEpaule, angleCoude, anglePoignet);
}

void Bras::move(const double x, const double y)
{ 
  double a,b,c,A,B,theta;
  
  a = AVANT_BRAS_LONGUEUR;
  c = BRAS_LONGUEUR;
  b = sqrt(x*x + y*y);
  
  // Loi des cosinus
  A = acos(  -(a*a - b*b - c*c)   /   (2*b*c) );
  B = acos(  -(b*b - a*a - c*c)   /   (2*a*c) );
  
  theta = atan(y/x);
 
  float anglePoignet = Bras::radToDeg( float(B) );
  float angleCoude   = Bras::radToDeg( float(A+theta) );
  
  setAngle(Poignet, anglePoignet);
  setAngle(Coude, angleCoude);
  
}

void Bras::fullExtension()
{
  setAngle(Poignet, 180);
}

void Bras::repos(const MoveOrder order)
{
  syncMove(90, 90, 0, order);
}

void Bras::saisir(const Objet objet)
{
  float angle;
  switch(objet)
  {
    case Batterie: angle = 30; break;
    case Oeuf:     angle = 70; break;
  }
  
  setAngle(Main,angle);
}

void Bras::relacher()
{
  setAngle(Main,100);
}

void Bras::ranger()
{
  repos(CPE);
  relacher();
}

void Bras::ressortir(const Objet objet)
{
  relacher();
  repos();
  saisir(objet);
}

