
/**************************************** 
 * Include Libraries 
 ****************************************/ 
#include <WiFi.h> 
#include "DHT.h" 
#include <WiFiClient.h>
#include <HTTPClient.h>

/**************************************** 
 * Define Constants 
 ****************************************/ 
#define WIFISSID "TestWiFiNet" //WIFI SSID aqui 
#define PASSWORD "1234567890" // WIFI pwd 
#define TOKEN "BBFF-0O3RvdSvbpw5m6FH4Ggz8bzWJNb9tT" // Ubidots TOKEN name el mismo que usamos en clase 

#define VARIABLE_LABEL_temp "temperatura" // Variable Temperatura 
#define VARIABLE_LABEL_hum "humedad" // Variable Humedad 
#define DEVICE_LABEL "esp32" // Nombre del dispositivo a crear 

#define pin13 13 
 
// Space to store values to send 
char str_temp[10]; 
char str_hum[10]; 

char payload[200]; // Leer y entender el payload aqui una de tantas referencias "https://techterms.com/definition/payload"

DHT dht1(pin13, DHT11);    //El sensor de temp y humedad azul. 

char humedadUrl[] = "http://industrial.api.ubidots.com/api/v1.6/variables/634054128472cb6ff6cb3578/values";
char temperaturaUrl[] = "http://industrial.api.ubidots.com/api/v1.6/variables/63404d8dfe13941d8496d0cc/values";

WiFiClient client;
HTTPClient httpClient;

/**************************************** 
 * Funciones auxiliares 
 ****************************************/ 
 
/* setTemperaturePayload */
void setTemperaturePayload(char* payload) {
  float t1 = dht1.readTemperature(); 
  Serial.print("temperatura: ");
  Serial.println(t1); // Imprime temperatura en el serial monitor  
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(t1, 4, 2, str_temp);
  sprintf(payload, "{\"value\": %s}", str_temp);
}

/* setHumidityPayload */
void setHumidityPayload(char* payload) {
  float h1 = dht1.readHumidity(); 
  Serial.print("humedad: ");
  Serial.println(h1); // Imprime humedad en el serial monitor  
  /* numero maximo 4 precision 2 y convierte el valor a string*/ 
  dtostrf(h1, 4, 2, str_hum);
  sprintf(payload, "{\"value\": %s}", str_hum);
}

void sendPostRequest(char* url, char* payload) {
  int httpStatusCode;

  httpClient.begin(client, url);
 
  httpClient.addHeader("Content-Type", "application/json");
  httpClient.addHeader("X-Auth-Token", TOKEN);

  httpStatusCode = httpClient.POST(payload);
  Serial.print("HTPP Status code: ");
  Serial.println(httpStatusCode);
  Serial.print("Send ");
  Serial.println(payload);

  Serial.println(httpClient.getString());

  // Free resources
  httpClient.end();
}

void http_post()
{
  setTemperaturePayload(payload);
  sendPostRequest(temperaturaUrl, payload);
  delay(2000);
  setHumidityPayload(payload);
  sendPostRequest(humedadUrl, payload);
  delay(13000);
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
 
  // Sensor de temp y humedad 
  dht1.begin(); 
} 

void loop() 
{ 
  http_post();
}
 