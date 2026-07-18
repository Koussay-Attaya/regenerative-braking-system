/*hadii il partie mta33 il controle de vitesse */

#include <ESP32Servo.h> 
Servo ESC;
int vitesseDemandee;
const int hallPin = 32;
volatile unsigned long lastPulseTime = 0;
volatile unsigned long timeBetweenPulses = 0;

unsigned long lastDisplayTime = 0;
float speedKmh = 0;

const float wheelCircumference = 0.036;
const unsigned long timeout = 1000000;
const unsigned long debounceTime = 500;
#define niv1 5
#define niv2 2
#define niv3 15
#define  accelerateur 39 




int vitmax = 20; // Valeur par défaut

void onPulse() {
  unsigned long now = micros();
  if ((now - lastPulseTime) > debounceTime) {
    if (lastPulseTime > 0) {
      timeBetweenPulses = now - lastPulseTime;
    }
    lastPulseTime = now;
  }
}







// partiemta3 il referoidissement
#define ref 18
#define ventillateure 19 
//#define chauf 5
#define lm35 36






//partie mta3 il regeneration
#include <Wire.h>
#include <MPU9250_asukiaaa.h>
#include <Adafruit_BMP280.h>

MPU9250_asukiaaa mySensor;
Adafruit_BMP280 bmp; // I2C par défaut



#define gachette1 13
#define gachette2 12
#define gachette3 14
#define frein 4 
#define tran_alim 23 





void setup() {
// hadha il set up mta3 il control de vitesse 
 ESC.attach(27,1000,2000);
  Serial.begin(115200);
  pinMode(hallPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallPin), onPulse, FALLING); 
//hadhi il partie mta3 il referoidissement 
 pinMode(5,OUTPUT);
 pinMode(4,OUTPUT);
 pinMode(3,OUTPUT);
 pinMode(36,INPUT);



//hadhi il partie set up mta3 il regeneration 
  pinMode (4,INPUT);
  pinMode (13,OUTPUT);
  pinMode (12,OUTPUT);
  pinMode (14,OUTPUT);
  pinMode (23,OUTPUT);
  Serial.begin(115200);
  Wire.begin();

  // Initialisation MPU9250
  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  // Initialisation BMP280
  if (!bmp.begin(0x76)) { // 0x76 ou 0x77 selon module
    Serial.println("BMP280 non détecté !");
    while (1);
  }

  Serial.println("GY-91 prêt !");







}

void loop() {

// hadha code il controll de vitesse   
  int acceleration = analogRead(accelerateur);
  bool btnNiv1 = !digitalRead(niv1); // Inversion car PULLUP
  bool btnNiv2 = !digitalRead(niv2);
  bool btnNiv3 = !digitalRead(niv3);

  // 2. Sélection du niveau de vitesse
  if ((btnNiv1==1) && (btnNiv2==0) && (btnNiv3==0)) {
    vitmax = 5;
  } 
  else if ((btnNiv1==0) && (btnNiv2==1) && (btnNiv3==0)) {
    vitmax = 10;
  } 
  else if ((btnNiv1==0) && (btnNiv2==0) && (btnNiv3==1)) {
    vitmax = 15;
  }
  

  // 3. Conversion et limitation
  vitesseDemandee = map(acceleration, 1000, 2950, 0, vitmax);
  vitesseDemandee = constrain(vitesseDemandee, 0, vitmax); // Double sécurité /////vitesse illi matloub annah tkoun il scooteure temchii fiih (vitesse demander )
  
  // 4. Envoi au moteur
  int pwmOutput = ((vitesseDemandee*255)/100);
  ESC.write(pwmOutput);

  // 5. Calcul vitesse réelle (identique à votre version)
  unsigned long nowMicros = micros();
  unsigned long nowMillis = millis();

  if (timeBetweenPulses > 0) {
    float rpm = 60.0 * 1000000.0 / timeBetweenPulses;
    float rawSpeed = rpm * wheelCircumference * 60.0 / 1000.0;
    speedKmh = 0.9 * speedKmh + 0.1 * rawSpeed;         ///// il vitesse illi temchii fiih il scooteure ( vitesse actuelle )
    if (speedKmh > 150.0) speedKmh = 0;
  }

  if ((nowMicros - lastPulseTime) > timeout) speedKmh = 0;

  // 6. Affichage
  if ((nowMillis - lastDisplayTime) > 200) {
    lastDisplayTime = nowMillis;
     Serial.print(btnNiv1 );
      Serial.print(btnNiv2);
       Serial.println(btnNiv3);
    Serial.print("Vitesse = ");
    Serial.print(speedKmh, 2);
    Serial.println(" km/h");
   // Serial.println(pwmOutput);
    Serial.print("Commande: ");
    Serial.println(vitesseDemandee);
    Serial.println(analogRead(39));
     delay(500);
  }



//hadhi il partie mta3 il referoidissement 
 uint16_t val;
    double dat;
    val=analogRead(lm35);//Connect LM35 on Analog 36
    dat = (double) val * (5/10.24);
    if(dat>=40){
        digitalWrite(ventillateure,HIGH);
        Serial.print("referoidissement partiel ! ");
    }
    else if(dat>=50){
        digitalWrite(ventillateure,HIGH);
        digitalWrite(ref,HIGH); 
        Serial.print("referoidissement total !!!!");
    }
    
    
    
    else {
        digitalWrite(ventillateure,LOW);
        digitalWrite(ref,LOW); 
        }

        
    Serial.print("Tep:"); //Display the temperature on Serial monitor
    Serial.print(dat);
    Serial.println("C");
    delay(500);







//hadhi il partie mta33 il regeneration
if((frein==1) && (speedKmh>=30))
  {
     digitalWrite(gachette1,HIGH);
     digitalWrite(gachette2,HIGH);
     digitalWrite(gachette3,HIGH);
     digitalWrite(tran_alim,HIGH);

  }

else if((vitesseDemandee<speedKmh) && (speedKmh>=30))
  {
        digitalWrite(gachette1,HIGH);
     digitalWrite(gachette2,HIGH);
     digitalWrite(gachette3,HIGH);
     digitalWrite(tran_alim,HIGH);
  }
else if((vitesseDemandee<speedKmh) && ((mySensor.accelZ())>=0.5 ))// il condition hadhi bach wa9eet illi nabda fi habta ma nest7a99ech il condition mta3 30km/h bach te5dem il regeneration
  {
       digitalWrite(gachette1,HIGH);
 
     digitalWrite(gachette2,HIGH);
     digitalWrite(gachette3,HIGH);
     digitalWrite(tran_alim,HIGH);
  }
else if( (mySensor.accelZ())>=0.8 )
  {
       digitalWrite(gachette1,HIGH);
     digitalWrite(gachette2,HIGH);
     digitalWrite(gachette3,HIGH);
     digitalWrite(tran_alim,HIGH);
  }

else {
        digitalWrite(gachette1,LOW);
     digitalWrite(gachette2,LOW);
     digitalWrite(gachette3,LOW);
     digitalWrite(tran_alim,LOW);
    }


 mySensor.accelUpdate();
  mySensor.gyroUpdate();
  mySensor.magUpdate();

  // Lecture Accéléromètre
  Serial.print("Accel X: "); Serial.print(mySensor.accelX());
  Serial.print(" Y: "); Serial.print(mySensor.accelY());
  Serial.print(" Z: "); Serial.println(mySensor.accelZ());

}
