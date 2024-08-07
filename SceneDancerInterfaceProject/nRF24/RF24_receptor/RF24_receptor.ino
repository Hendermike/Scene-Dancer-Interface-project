/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
uint8_t address[][6] = {"Main", "Suit"};

void setup() {
  Serial.begin(9600);
  RFSetup();

}
void loop() {
  if (radio.available()) {
    uint16_t text=0;
    radio.read(&text, sizeof(text));
    Serial.println(String(text));
  }
}

void RFSetup(void) {
  radio.begin();
  radio.openReadingPipe(1, address[0]);
  radio.setChannel(125);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
}
