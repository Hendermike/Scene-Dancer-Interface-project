#include <SPI.h>
#include <SD.h>

int currentMeasure;

const int chipSelect = 4;
unsigned long referenceMillis;

void setup() {
  Serial.begin(9600);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
}


void loop() {
    // put your main code here, to run repeatedly:
    currentMeasure = analogRead(A0);
        
    // SD card logging related
    SDLog(currentMeasure);
   
}

void SDLog(int measure) {
  // make a string for assembling the data to log:
  String dataString = "";
  dataString += String(millis() - referenceMillis)  + ": ";

  // read three sensors and append to the string:
  for (int i = 0; i < 1; i++) {
    dataString += String(measure);
    if (i < 0) {
      dataString += ",";
    }
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
}
