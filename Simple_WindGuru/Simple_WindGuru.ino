
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
//#include <MD5Builder.h>
MD5Builder md5;

const char* ssid     = "lu4ult";
const char* password = "lucaprodan";

const char* stationId = "lu4ult";
const char* stationPassword = "01234_hola";

unsigned long previousMillis = 0;
unsigned long lastWindguruRequest = 0;

float temperature=18.2, humidity=24;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    if(currentMillis - lastWindguruRequest > 60*1000) {
      
      if(putToWindguru(stationId,stationPassword,temperature,humidity)) {
        lastWindguruRequest = millis(); 
        Serial.println("Éxito!");
      }
      else  {
        Serial.println("Ups! Algo salió mal...\nDescomente 'SERIAL_DEBUG' para ver información en el monitor serie.");
      }
    }
  }
}


//Api documentation:
//https://stations.windguru.cz/upload_api.php
//Request example:
//http://www.windguru.cz/upload/api.php?uid=stationXY&salt=20180214171400&hash=c9441d30280f4f6f4946fe2b2d360df5&wind_avg=12.5&wind_dir=165&temperature=20.5

//To-Do: agregar parámetros restantes
//bool putToWindguru (String uid, String station_password, unsigned long interval, float wind_avg, float wind_max, float wind_min, float wind_direction, float temperature, float rh, float mslp, float precip, unsigned long precip_interval) {
bool putToWindguru (String uid, String station_password, float temperature, float humidity) {
  //#define SERIAL_DEBUG                                        //Descomentar para ver mensajes de debug en el Monitor serie.                                     
  
  String salt = String(random(1000,9999));                    //Generamos un número aleatorio de 4 cifras que se usará para generar el hash md5.
  md5.begin();                                                //Este número aleatorio debe ser distinto en cada petición, por ende también el hash.
  md5.add(salt+uid+station_password);
  md5.calculate();
  String hash = md5.toString();
  
  String url = "http://www.windguru.cz/upload/api.php?";    

  url += "uid=" + uid;
  url += "&salt=" + salt;
  url += "&hash=" + hash;
  url += "&temperature=" + String(temperature);
  url += "&rh=" + String(humidity);
  
  #ifdef SERIAL_DEBUG
  Serial.println("Salt: "+salt);
  Serial.println(salt+uid+station_password);
  Serial.println("Hash: "+hash);  
  Serial.println(url);
  #endif

  WiFiClient client;
  HTTPClient http;
  String payload;
  
  if(http.begin(client, url)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
      #ifdef SERIAL_DEBUG
      Serial.println(payload);
      #endif
      }
    else {
      #ifdef SERIAL_DEBUG
      Serial.printf("[HTTP] Error: %s\n", http.errorToString(httpCode).c_str());
      #endif
    }
    http.end();
  }
  if(payload == "OK")               //WindGuru api returns "OK" on success
    return 1;
  else
    return 0;
}
