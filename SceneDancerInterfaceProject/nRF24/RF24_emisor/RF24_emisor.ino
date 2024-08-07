/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE 7
#define CNS 8

const char text[] = "001";
uint16_t testPacket = 100;

RF24 radio(CE, CNS); // CE, CSN
uint8_t address[][6] = {"Main", "Suit"};
void setup() {
  RFSetup();
}
void loop() {
  uint16_t msg = analogRead(A0);
  radio.write(&testPacket, sizeof(testPacket));
  delay(500);
}

void RFSetup(void) {
  radio.begin();
  radio.openWritingPipe(address[0]);
  radio.setChannel(125);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
}
