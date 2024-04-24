/* 
Proyecto: Vantage 2210 version 0.3 Ultima mejora: 12/04/24
Descripcion: Leer datos modbus Vantage. IoT Cloud: Datacake, Monitoreo y OTA: Arduino Cloud
Autor: Isaac BG

Funciones principales:
-WiFi --OK
-Arduino Cloud connection with boolean variable --OK
-Comunicacion Modbus Vantage y conversion --OK
-HTTP API --en progreso
-Bubble   --
-Datacake --
-Supabase --
-Backendless --
-Adicionales: --
-Mejoras en estructura de codigo:

*/
#include <WiFi.h>
#include <Arduino.h>
#include "thingProperties.h"
#include <ModbusMaster.h>
#include <ArduinoHttpClient.h>  
#define led_interno 2
#define DatacakeURL           "https://api.datacake.co/integrations/api/4074155d-15d6-4cdb-8107-3f903d5e225d"  // El "/" final se remueve para usarlo en la funcion POST
#define DatacakeDevice        "6fb7c285-953e-4e2a-a041-84040ebfdfed"  //Identificador de dispositivo datacake

ModbusMaster vantage; //Crear objeto Modbus
WiFiClient wifi;  //declarar un objeto wifi depndiente de la libreria WiFi101.h incluida en ArduinoIoTcloud
HttpClient httpClient = HttpClient(wifi, DatacakeURL);

static unsigned long time_now = millis();



void setup() {
 
  pinMode(led_interno,OUTPUT);   // Inicializar pin GPIO2 conectado a LED interno
  Serial.begin(9600);
  Serial2.begin(38400, SERIAL_8N1, 16, 17);
  delay(1500);
  vantage.begin(1, Serial2); //Configurar comunicacion con slave id #1 y Serial2
  initProperties();// Defined in thingProperties.h 
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);  // Connect to Arduino IoT Cloud
  //WiFi.begin(SECRET_SSID,SECRET_OPTIONAL_PASS);  //Inicializar WiFi
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

//******Read Vantage Modbus Holding register function with type convertion******
float leerVantageFlujo() {       
  uint8_t   status;
  float     flujo;
  uint16_t  data[2];

  status = vantage.readHoldingRegisters(10, 2); //Leer (2) registros de 16bit, en direccion 10 (flujo)

  if (status == vantage.ku8MBSuccess)
  {
    
    data[1]=vantage.getResponseBuffer(0); //Formato little-endian: data[1]=Exponente y signo, data[0]=mantisa formato IEE 754 HEX
    data[0]=vantage.getResponseBuffer(1);

    memcpy(&flujo, &data, sizeof(data)); //Copiar valores en data(2x16bits) a variable flujo tipo float(32bits)
       
  }
  else {

    flujo=0.11; //Indica que no se pudo leer dato

  }

  return flujo;
  
}

void loop() {
  ArduinoCloud.update();

  

  //Este codigo se ejecutara solamente si existe conexion a cloud (internet) y WiFi object creado para HTTP
  if (ArduinoCloud.connected() && WiFi.isConnected()){

    digitalWrite(led_interno, HIGH);
    if(millis() - time_now >= 5000ul) {  //Timer activado cada 5 segundos, leer flujo
      time_now += 5000;
      Serial.println(leerVantageFlujo());
    }

  }
  else  { 
    //mostrar status de conexiones
    Serial.println("Conexion internet no activa aun");
    delay(1000);

  }

}


void enviarDatacake(float flujo){
/* Cuerpo muestra del mensaje a enviar
 {
	"device": "6fb7c285-953e-4e2a-a041-84040ebfdfed",
	"flujo": 10.5,
	"nivel": 5.1,
	"totalizador": 1
}

*/
  float nivel = 1.0; //Variables prueba
  int totalizador = 1; //Varieable prueba
  
  String contentType = "application/json";
  String jsonBody = "{\"device\":\"";
  jsonBody += DatacakeDevice;
  jsonBody += "\"";
  jsonBody += ",\"flujo\":";
  jsonBody += flujo;
  jsonBody += ",\"nivel\":";
  jsonBody += nivel;
  jsonBody += ",\"totalizador\":";
  jsonBody += totalizador;
  jsonBody += "}";
  // see what you assembled to send:
  Serial.print("PUT request to server: ");
  Serial.println(jsonBody);
  Serial.print("json command to server: ");

  // make the POST request to Datacake:
  httpClient.post("/", contentType, jsonBody); 




}

void enviarSupabase(float flujo){  

}

void enviarBackendless(float flujo){  

}

void enviarBubble(float flujo){  

}




//Evento activado al moverse la variable Switch1 en cloud
void onSwitch1Change()  {
  // Add your code here to act upon Led change
  if (switch1){
    digitalWrite(led_interno,HIGH);
    
  }
  else {
    digitalWrite(led_interno,LOW);

  }
}  

  

