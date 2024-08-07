
#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
SoftwareSerial BT(10,11);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth

bool traffic_mode = false;

bool start_hit = false;
int start_hit_counter = 0;
const int start_stream_length = 7;
char start_stream[start_stream_length] = {'S','T','A','R','T','\r','\n'};
int samples[3] = {0, 0, 0};


bool start_check(char c) {
  if (c == start_stream[start_hit_counter]) {
    start_hit_counter += 1;
    
    if (start_hit_counter == start_stream) {
      
      start_hit = true;
      Serial.print("STREAMING STARTED");
      traffic_mode = true;
      start_hit_counter = 0;
    }
    return true;
  } else {
    start_hit_counter = 0;
    return false;
  }
}

void setup() {
  //BT.begin(9600);       // Inicializamos el puerto serie BT (Para Modo AT 2)
  Serial.begin(9600);
}


void loop() {

  /*for (int i = 2; i>0; i--) {
    samples[i] =samples[i-1];
  }
  samples[0] = analogRead(A0);
  */
  
  //int value = int(0.5*samples[0] + 0.3*samples[1] + 0.2*samples[2]);
  Serial.println(analogRead(A0));
  //Serial.println(value);
  delay(30);
  /*if (traffic_mode) {
    char cstr[16];
    itoa(analogRead(A0), cstr, 10);
    BT.write(cstr);
    BT.write("\r\n");
  } else {
    if (BT.available()) {
      int incoming_char = BT.read();
      start_check(incoming_char);
    }
  }*/
  
}
