#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
SoftwareSerial BT(9,10);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth

int currentMeasure;

const int chipSelect = 4;
unsigned long referenceMillis;

bool traffic_mode = false; // Indicador de estado de tráfico de data

const int start_stream_length = 7; // Largo del comando de inicio de tráfico
char start_stream[start_stream_length] = {'S','T','A','R','T','\r','\n'}; // Comando de inicio de tráfico
bool start_hit = false; // Indicador de detección de comando de inicio de tráfico

 
// Chequeo caracter por caracter del comando de inicio de tráfico, si un caracter intermedio no coinide
// el contador de coincidencias vuelve a cero. Si el ingresado coinicde con el evaluado, el contador incrementa.
// Si el ingresado es el término del comando, reinicia el contador.

int start_hit_counter = 0; // Contador de caracteres para la detección del comando de inicio
bool start_check(char c) {
  if (c == start_stream[start_hit_counter]) {
    start_hit_counter += 1;
    if (start_hit_counter == start_stream_length) {
      start_hit = true;
      traffic_mode = true; // Inicia el modo de tráfico
      start_hit_counter = 0; // Reinicia el contador
      Serial.print("STREAMING STARTED");
    }
    return true;
  } else {
    start_hit_counter = 0;
    return false;
  }
}

void setup() {
  BT.begin(9600);       // Inicializamos el puerto serie BT (Para Modo AT 2)
  Serial.begin(9600);
}


void loop() {
    // put your main code here, to run repeatedly:
    currentMeasure = analogRead(A0);
    
    // Streaming related   
    if (traffic_mode) { // En modo tráfico
      if (BT.available()) { // Descargando FIFO de entrada
        int incoming_char = BT.read();
      } else { // Subiendo medición a FIFO de salida
        // Envío del mensaje
        char cstr[16];
        itoa(currentMeasure, cstr, 10);
        BT.write(cstr);
        BT.write("\r\n");
      } 
    } else { // Modo de espera
      if(BT.available()) {
        int incoming_char = BT.read();
        Serial.write(incoming_char);
        if (!start_hit) {
          start_check(incoming_char);
        }
      }
      if(Serial.available()) {
        BT.write(Serial.read());
      }
    }
   
}
