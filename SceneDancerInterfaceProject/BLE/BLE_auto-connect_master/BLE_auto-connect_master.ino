#include <SoftwareSerial.h>   // Incluimos la librería  SoftwareSerial  
SoftwareSerial BT(10,11);    // Definimos los pines RX y TX del Arduino conectados al Bluetooth

unsigned long last_connection_try;
unsigned long last_start_try;
unsigned long last_reception_millis;

bool inq_sended = false;
int i = 0;
char incoming_content[100] = {};
char inq[5] = {'+','I','N','Q','S'};
char connect_to_mac[10] = {'A','T','+','C','O','N','N','-','\r','\n'};
char start_traffic[7] = {'S','T','A','R','T','\r','\n'};

bool on_traffic = false;
bool auto_connect = true;

char index_window[4];
void index_window_update(char c) {
  index_window[0] = index_window[1];
  index_window[1] = index_window[2];
  index_window[2] = index_window[3];
  index_window[3] = c;
  //Serial.println(String(char(index_window[0])) + String(char(index_window[1])) + String(char(index_window[2])) + String(char(index_window[3])));
}

const int mac_length = 14; // Largo de dirección mac
char target_mac[mac_length] = {'0','x','0','0','1','3','A','A','0','0','1','C','C','A'}; // Dirección mac buscada
bool mac_hit = false; // Indicador de detección de mac buscada


// Chequeo caracter por caracter de la dirección mac del esclavo objetivo (target_mac). Recupera el índice 
// de la dirección en el registro de dispositivos detectados mediante INQ.

char mac_index = 0; // Indice de la dirección mac en los registros del CC2541
int mac_hit_counter = 0; // Contador de coincidencias de caracteres de la dirección mac objetivo
unsigned long mac_hit_millis; // Referencia de la última detección de la dirección mac
bool mac_check(char c) {
  if (c == target_mac[mac_hit_counter]) { // El carácter recibido coincide 
    mac_hit_counter += 1;
    if (mac_hit_counter == 1) { // Guarda el indice de la mac en detección
        mac_index = index_window[1];
    } else if (mac_hit_counter == mac_length) {
        mac_hit = true;
       
        connect_to_mac[7] = mac_index; // Actualiza el comando de inicio de sesión con el indice de la dirección mac objetivo
        
        //Serial.println("MAC DETECTED : " + String(mac_index));
        
        
        //Serial.println(connect_to_mac);
        
        last_connection_try = millis(); // Referencia del último inteto de conexión
        mac_hit_counter = 0; // Se reinicia el contador de detección

        return true;
    }
  } else {
    mac_hit_counter = 0;
  }
  return false;
}

// Chequeo caracter por caracter de la respuesta 'Connected' desde el CC2541
bool connected_hit = false;
int connected_hit_counter = 0;
const int connected_stream_length = 9;
char connected_stream[connected_stream_length] = {'C','o','n','n','e','c','t','e','d'};
bool connected_check(char c) {
  if (c == connected_stream[connected_hit_counter]) {
    connected_hit_counter += 1;
    
    if (connected_hit_counter == connected_stream_length) {
      
      connected_hit = true;
      //Serial.println(target_mac);
      Serial.println("CONN DETECTED");
      
      last_start_try = millis();

      connected_hit_counter = 0;
    }
    
    return true;
  } else {
    connected_hit_counter = 0;
    return false;
  }
}

 
void setup()
{
  BT.begin(9600);       // Inicializamos el puerto serie BT (Para Modo AT 2)
  Serial.begin(9600);   // Inicializamos  el puerto serie  
}

unsigned long last_inq_millis;
void loop()
{
  // Auto-connect
  if (auto_connect) {
  
    if(BT.available()) {   // Si llega un dato por el puerto BT
      //i+=1;
      int incoming_char = BT.read();
      Serial.write(incoming_char);
      //Serial.println(String(i)+" : "+char(incoming_char));
      index_window_update(incoming_char);
      if (!mac_hit) {
        mac_check(incoming_char);
      } else if (!connected_hit) {
        connected_check(incoming_char);
      }
      
    } else {
      
      if (inq_sended==false) {
        BT.write("AT+INQ\r\n");
        inq_sended = true;
        Serial.println("INQ TRY");
        last_inq_millis = millis();
      } else if (mac_hit && !connected_hit) {
        if (millis() - last_connection_try > 10000) {
          BT.write(connect_to_mac);
          last_connection_try = millis();
          Serial.println("CONN TRY");
        }
      } else if(mac_hit && connected_hit) {
         if (millis() - last_start_try > 10000) {
          for (int k = 0; k < 3; k++) {
            BT.write(start_traffic);
          }
          Serial.println("START TRY");
          auto_connect = false;
        }
      } else if (millis() - last_inq_millis > 5000 && !mac_hit) { // Se dan 15 segundos para encontrar la dirección mac en la lista
        inq_sended = false;
      }
      //Serial.println(" . ");    
    }
    
  } else {
    
    // Tráfico
    //Serial.println("ON TRAFFIC");
    
    if (BT.available()) { //TODO: implementar reintentos de conexión.
      Serial.write(BT.read());// Recepción contenido restante
      last_reception_millis = millis();
    } else {
      /*if (!on_traffic) { // Aún no se trafican datos
        char start_traffic[7] = {'S','T','A','R','T','\r','\n'};
        for (int k = 0; k < 3; k++) {
          BT.write(start_traffic);
        }
        Serial.println("START TRY");
        on_traffic = true;
        
      }*/
    }

    if (millis() - last_reception_millis > 5000) {
        Serial.println("ON TRAFFIC");
        last_reception_millis = millis();
    }
    /*if (millis() - last_reception_millis > 10000) {
        auto_connect = true;
        Serial.println("BACK TO AUTO-CONNECT");
    }*/
    
  }
  
}

//00:13:AA:00:1C:CA (slave addr)
