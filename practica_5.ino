/**************************************** 
 * Include Libraries 
 ****************************************/ 
#include <WiFi.h> 
#include <PubSubClient.h> 
#include "DHT.h" 

/**************************************** 
 * Define Constants 
 ****************************************/ 
#define WIFISSID "TestWiFiNet" //WIFI SSID aqui 
#define PASSWORD "1234567890" // WIFI pwd 
#define TOKEN "BBFF-0O3RvdSvbpw5m6FH4Ggz8bzWJNb9tT" // Ubidots TOKEN name el mismo que usamos en clase 
#define MQTT_CLIENT_NAME "BBFF-ac594aa7f390623ae2ee89e3fd21624af1b" //ID del cliente, 8 a 12 chars alfanumericos (ASCII), debe ser random y unico dif a otros devices 
 
#define VARIABLE_LABEL_temp "temperatura" // Variable Temperatura 
#define VARIABLE_LABEL_hum "humedad" // Variable Humedad 
#define DEVICE_LABEL "esp32" // Nombre del dispositivo a crear 

#define pin13 13 
 
char mqttBroker[]  = "industrial.api.ubidots.com"; 
char payload[200]; // Leer y entender el payload aqui una de tantas referencias "https://techterms.com/definition/payload" 
char topic[150]; //Espacio para el nombre del topico 
 
// Space to store values to send 
char str_temp[10]; 
char str_hum[10]; 
 
DHT dht1(pin13, DHT11);    //El sensor de temp y humedad azul. 
 
/**************************************** 
 * Funciones auxiliares 
 ****************************************/ 
WiFiClient ubidots; 
PubSubClient client(ubidots); 
 
void callback(char* topic, byte* payload, unsigned int length) 
{ 
  char p[length + 1]; 
  memcpy(p, payload, length); 
  p[length] = 0; 
  String message(p); 
  Serial.write(payload, length); 
  Serial.println(topic); 
} 
 
void reconnect() { 
  // Loop until we're reconnected 
  while (!client.connected()) 
  { 
    Serial.println("Attempting MQTT connection..."); 
     
    // Attemp to connect 
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) 
    { 
      Serial.println("Connected"); 
    } 
    else 
    { 
      Serial.print("Failed, rc="); 
      Serial.print(client.state()); 
      Serial.println(" try again in 2 seconds"); 
      // Wait 2 seconds before retrying 
      delay(2000); 
    } 
  } 
} 
 
/**************************************** 
 * Main Functions 
 ****************************************/ 
void setup() { 
  Serial.begin(115200); 
  WiFi.begin(WIFISSID, PASSWORD); 
 
   
  Serial.println(); 
  Serial.print("Wait for WiFi..."); 
   
  while (WiFi.status() != WL_CONNECTED) 
  { 
    Serial.print("."); 
    delay(500); 
  } 
   
  Serial.println(""); 
  Serial.println("WiFi Connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
  client.setServer(mqttBroker, 1883); 
  client.setCallback(callback);   
 
  // Sensor de temp y humedad 
  dht1.begin(); 
} 
 
void loop() { 
  if (!client.connected()) 
  { 
    reconnect(); 
  } 
 
  float t1 = dht1.readTemperature(); 
  float h1 = dht1.readHumidity();

  // Publica en el topic de temperatura 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); // Cleans the payload 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_temp); // Adds the variable label 
 
  Serial.println(t1); // Imprime temperatura en el serial monitor  
 
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(t1, 4, 2, str_temp); 
    
  sprintf(payload, "%s {\"value\": %s", payload, str_temp); // formatea el mensaje a publicar 
  sprintf(payload, "%s } }", payload); // cierra el mensaje 
  Serial.println("Publicando temperatura en Ubidots cloud");  
 
  boolean errorT;
  errorT = client.publish(topic, payload); 
  if(errorT == 0)
  {
    Serial.println("Error al publicar en Ubidots cloud");
  }

  delay(2000); // 2 segundos en milisegundos entre publicaciones en ubidots 
  
  // Publica en el topic de humedad 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); // Cleans the payload 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_hum); // Adds the variable label 
   
 
  Serial.println(h1); // Imprime temperatura en el serial monitor  
 
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(h1, 4, 2, str_hum); 
   
  sprintf(payload, "%s {\"value\": %s", payload, str_hum); // formatea el mensaje a publicar 
  sprintf(payload, "%s } }", payload); // cierra el mensaje 
  Serial.println("Publicando humedad en Ubidots cloud");  

  boolean errorH;
  errorH = client.publish(topic, payload); 
  if(errorH == 0)
  {
    Serial.println("Error al publicar en Ubidots cloud");
  }
   
  client.loop(); 
  delay(13000); // 5 segundos en milisegundos entre publicaciones en ubidots 
} 
 