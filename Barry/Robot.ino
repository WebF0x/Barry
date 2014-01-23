#include "Robot.h"

Robot::Robot(Bras* bras, const int speedPin, const int avancerPin, const int reculerPin, const int soundPin0, const int soundPin1, 
                         const int barSensorPin, const int lanceBallePin, const int drapeauPin) :
  m_bras(bras), m_speedPin(speedPin), m_avancerPin(avancerPin), m_reculerPin(reculerPin), 
  m_soundPin0(soundPin0), m_soundPin1(soundPin1), m_barSensorPin(barSensorPin), m_lanceBallePin(lanceBallePin), m_drapeauPin(drapeauPin)
{
  //Motor pins
  pinMode(m_speedPin, OUTPUT);
  pinMode(m_avancerPin, OUTPUT); 
  pinMode(m_reculerPin, OUTPUT); 
 
  //Sound Pins
  pinMode(m_soundPin0, OUTPUT);
  pinMode(m_soundPin1, OUTPUT);
  
  //Bar sensor Pin
  pinMode(m_barSensorPin, INPUT);
  
  setSpeed(255);
  m_position = 1;  //Pour fitter dans la zone de départ
  m_movementState = Arreter;
  
  m_epreuve = Debut;
  
  m_servoLanceBalle = new Servo();
  m_servoLanceBalle->attach(m_lanceBallePin, 600, 2400);
  m_servoDrapeau = new Servo();
  m_servoDrapeau->attach(m_drapeauPin, 600, 2400);  
  
  m_bras->repos();
  m_bras->syncMove(180, 180, 180, ECP); //Deplier le bras par en arriere vers la gauche
  
  Serial.begin(9600);
}

Robot::~Robot()
{
  delete m_bras;
}

void Robot::avancer()
{
  digitalWrite(m_avancerPin, HIGH);
  digitalWrite(m_reculerPin, LOW);
  
  m_movementState = Avancer;
  Serial.println("J'avance!");
}

void Robot::reculer()
{ 
  digitalWrite(m_avancerPin, LOW);
  digitalWrite(m_reculerPin, HIGH);

   m_movementState = Reculer;
   Serial.println("Je recule!");  

}

void Robot::arreter()
{  
  digitalWrite(m_avancerPin, LOW);
  digitalWrite(m_reculerPin, LOW); 
  
  Serial.println("J'arrete");
  m_movementState = Arreter; 
}

void Robot::setMovementState(MovementState newState)
{
  m_movementState = newState;
}

void Robot::action(const MovementState& movementState)
{
  setMusicToPlay(movementState);
  
  switch(movementState)
  {
    case Avancer:
       avancer();
       break;
    
    case Reculer:
       reculer();
       break;
     
    case Arreter:
       arreter();
       break;
      
    default:
       break; 
  }  
}

void Robot::setSpeed(const int vitesse)
{
  
  analogWrite(m_speedPin, vitesse);
  m_motorSpeed = vitesse;
}

int Robot::getSpeed()
{
    return m_motorSpeed;
}

int Robot::getPosition()
{
  return m_position;
}

MovementState Robot::getMovementState() 
{
  return m_movementState;
}

void Robot::updatePosition()
{
  switch (m_movementState)
  {
    case Avancer:
      m_position++;
      break;
    case Reculer:
      m_position--;
      break;
    case Arreter: break;  //Pas supposé arriver :|
  }
}

void Robot::ping()
{
  pinMode(m_barSensorPin, OUTPUT);
  
  digitalWrite(m_barSensorPin, LOW);
  delay(2);
  digitalWrite(m_barSensorPin, HIGH);
  delay(5);
  digitalWrite(m_barSensorPin, LOW);
  
  pinMode(m_barSensorPin, INPUT);
}

float Robot::microsToCm(int ms)
{
  return 0.034*float(ms) / 2;
}

float Robot::getDistFromSensor()
{
  int debut,delta;
  
  ping();
  debut = micros();
  
  do
  {
    delta = micros() - debut;
  }while(digitalRead(m_barSensorPin) != 0 && delta< 10000);
  
  if(delta < 10000)
  {
    return microsToCm(delta);
  }
  else
    return 0;
}

void Robot::evaluateBarSensorData(const float valeurLimite)
{
  static bool wasOverBar = false;
  
  //Calculer moyenne des distances
  float moy = 0;
  for(int i=0;i<5;i++)
  {
    moy += getDistFromSensor();  
  }
  moy /= 5;
  
  Serial.println(moy);
  
  if((moy > valeurLimite) == wasOverBar)  //C'est une trou et c'était une barre ou le contraire
  {
    updatePosition();
    wasOverBar = !wasOverBar;
    delay(50);
  }
}

int Robot::positionNeeded(const Epreuve epreuve)
{
  switch(epreuve)
  {
    case Debut:
      return 1;      
    case PrendreBatterie:
      return 12;
    case DetruireTour:
      return 19;
    case LeverBarriere:
      return 19;
    case LancerPingPong:
      return 27;        
    case BarreMetal:
      return 35;
    case DeposerBatterie:
      return 37;
    case PrendreOeuf:
      return 42;  //À tester
    case BloquerLaser:
      return 38;
    case ArreterMurphy:
      return 1;    
    case Fin:
      return 1;  
  }
}

Epreuve Robot::getEpreuve()
{
  return m_epreuve;
}

////////////////////////////////////////////ÉPREUVES////////////////////////////////////////////

void Robot::debut()
{
  m_bras->setAngle(Epaule, 90);   //Pour passer sous le pont
  
  m_epreuve = DetruireTour;
}

void Robot::detruireTour()
{ 
  Serial.println("Je detruit la tour de controle!");
  
  m_bras->syncMove(0, 0, 180, ECP);
  
  m_epreuve = LeverBarriere;
}

void Robot::leverBarriere()
{ 
  //À environ 40.6 cm
  ///Serial.println("Je leve la barriere!");
  
  ///m_bras->syncMove(90,90,90, PCE);
  ///m_bras->repos(PCE);
  
  //Mouvement en diagonal de bas-gauche vers haut-droite
//  m_bras->setAngle(Poignet, 130);
//  m_bras->syncMove(80,0,135, CPE); 
//  m_bras->syncMove(150,70,180, PCE);
//  
//  m_bras->repos(ECP);
//  
  m_epreuve = PrendreBatterie;
}

void Robot::prendreBatterie()
{
  Serial.println("Je ramasse la batterie!");
  
  m_bras->setAngle(Epaule, 180);
  m_bras->relacher();
  m_bras->move(36, -20);
  m_bras->saisir(Batterie);
  m_bras->repos(CEP);
  //m_bras->ranger();
  
  m_epreuve = LancerPingPong;
}

void Robot::lancerPingPong()
{ 
  Serial.println("J'elimine le bras droit de Murphy!");
  
  m_servoLanceBalle->write(90);  //Déclenche le mécanisme du lanceur
  delay(500);
  m_servoLanceBalle->write(0);
  
  m_epreuve = BarreMetal;
}

void Robot::barreMetal()
{ 
  Serial.println("Je court-circuite la bombe!");
  
  delay(2000);
  
  m_epreuve = DeposerBatterie;  
}

void Robot::deposerBatterie()
{ 
  Serial.println("J'active les generatrices!");
  
  //m_bras->ressortir(Batterie);
  m_bras->setAngle(Epaule, 0);
  m_bras->move(40.64, -15.
  
  );
  m_bras->relacher();
  m_bras->repos();
  
  float angleEpaule, angleCoude, anglePoignet;
  /*
    Trouver coordonnées cible
    
    Utiliser code de brassiere pour convertir en angles
  
  
  m_bras->syncMove(angleEpaule, angleCoude, anglePoignet);
  */
  
  m_epreuve = PrendreOeuf; 
  /**/m_epreuve = BloquerLaser; 
}

void Robot::prendreOeuf()
{ 
//  Serial.println("Je sauve l'otage!");
//  
//  
//  /*
//  float angleCoude, anglePoignet;
//  m_bras->syncMove(90, angleCoude, anglePoignet);
//    Trouver distance de l'oeuf
//    convertir en angleCoude et anglePoignet
//  */
//  
//  m_bras->setAngle(Epaule, 90);
//  m_bras->relacher();
//  m_bras->move(30, -17.8);
//  m_bras->saisir(Oeuf);
//  m_bras->ranger();
  
  m_epreuve = BloquerLaser;  
}

void Robot::bloquerLaser()
{ 
  Serial.println("Je détourne le Dr Murphy!");
  
  m_servoDrapeau->write(180);
  delay(3000);  //On doit attendre 2 secondes minimum
  m_servoDrapeau->write(0);
  
  m_epreuve = ArreterMurphy;  
}

void Robot::arreterMurphy()
{ 
//  Serial.println("Je capture Dr Murphy!");
//  
//  m_bras->syncMove(90, 180, 180);
//  m_bras->saisir(Batterie);
//  
  m_epreuve = Fin;
}

void Robot::fin()
{
  Serial.println("Mission accomplie!");
}

void Robot::setMusicToPlay(const MovementState& state)
{
  int X1=0, X0=0;
  switch(state)
  {
   case Avancer: break;
   case Arreter: X1 = 1; break;
   case Reculer: X1=1; X0=1; break;
  }
  digitalWrite(m_soundPin0, X0);
  digitalWrite(m_soundPin1, X1);
  
}

void Robot::test()
{  
  //lancerPingPong();
 
  
  while(true)
  {
    delay(1000);
    m_servoLanceBalle->write(120);
    delay(5000);
    m_servoLanceBalle->write(0);
  }

//  delete m_bras;
//  
//  action(Avancer);
//  
//  while(m_position<9)
//  robot->evaluateBarSensorData(12);   
//
//  action(Arreter);  
  
//  m_bras->setAngle(Poignet, 130);
//  delay(500);
//  m_bras->syncMove(80,0,115, CPE); 
//  delay(500);
//  m_bras->syncMove(150,70,115, PCE);
//  delay(300);
//  m_bras->saisir(Batterie);
//  m_bras->setAngle(Coude, 70);
//  delay(500);
//  m_bras->setAngle(Poignet, 130);
//  delay(500);
//  while(true);
}














