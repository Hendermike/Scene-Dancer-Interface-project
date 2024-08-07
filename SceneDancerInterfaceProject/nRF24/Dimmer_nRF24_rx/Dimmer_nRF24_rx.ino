//Libraries
#include <RBDdimmer.h>//https://github.com/RobotDynOfficial/RBDDimmer
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Parameters: Dimmer
const int zeroCrossPin  = 2;
const int acdPin  = 3;
int MIN_POWER  = 0;
int MAX_POWER  = 40;
int POWER_STEP  = 1;

//LPF
int samples[3] = {0, 0, 0};
int value = 0;

//Parameters: nRF24
RF24 radio(7, 8); // CE, CSN
uint8_t address[][6] = {"Main", "Suit"};

//Variables
int power  = 0;

//Objects
dimmerLamp acd(acdPin);

void setup(){
//Init Serial USB
Serial.begin(9600);
Serial.println(F("Initialize System"));
acd.begin(NORMAL_MODE, ON);

RFSetup();
}

void loop(){
  //testDimmer();
  for (int i = 2; i>0; i--) {
    samples[i] =samples[i-1];
  }
  samples[0] = analogRead(A0);
  
  //power=0;
  
  if (radio.available()) {
    uint16_t text=0;
    radio.read(&text, sizeof(text));
    
    samples[0] = text;
    value = int(0.33*samples[0] + 0.33*samples[1] + 0.33*samples[2]);
    //Serial.println(value);
    
    //Serial.println(text);
    power = int(100*value/600 - 20);
    Serial.println(power);
  }
  acd.setPower(power);
}

void testDimmer(){/* function testDimmer */ 
////Sweep light power to test dimmer
  for(power=MIN_POWER;power<=MAX_POWER;power+=POWER_STEP){
    acd.setPower(power); // setPower(0-100%);
      Serial.print("lampValue -> ");
      Serial.print(acd.getPower());
      Serial.println("%");
    delay(100);
  }

  for(power=MAX_POWER;power>=MIN_POWER;power-=POWER_STEP){
    acd.setPower(power); // setPower(0-100%);
      Serial.print("lampValue -> ");
      Serial.print(acd.getPower());
      Serial.println("%");
    delay(100);
  }
}

// nRF Setup
void RFSetup(void) {
  radio.begin();
  radio.openReadingPipe(1, address[0]);
  radio.setChannel(125);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  Serial.println(F("RF Initialized"));
}
