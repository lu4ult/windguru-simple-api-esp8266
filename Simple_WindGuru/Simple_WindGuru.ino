/*
 * Este programa genera valores aleatorio para humedad, temperatura y velocidad del viento y los publica en nuestra estación en windguru
 *
 * Desarrollado por: Lautaro lu4ult
 * https://github.com/lu4ult/windguru-simple-api-esp8266
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
//#include <MD5Builder.h>
MD5Builder md5;

const char* ssid     = "wifi-ssid";
const char* password = "wifi-password";

const char* stationId = "id";
const char* stationPassword = "password";


unsigned long previousMillis = 0;
unsigned long lastWindguruRequest = 0;

float temperature, humidity;
int vientoBase, vientoRacha;

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
      temperature=random(15.0,25.5);
      humidity=random(30,35);
      vientoBase=random(1,5);
      vientoRacha=random(10,15);

      if(putToWindguru(stationId,stationPassword,temperature,humidity,vientoBase,vientoRacha)) {
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
bool putToWindguru (String uid, String station_password, float temperature, float humidity, int wind_avg, int wind_max) {
  #define SERIAL_DEBUG                                        //Descomentar para ver mensajes de debug en el Monitor serie.

  String salt = String(random(1000,9999));                    //Generamos un número aleatorio de 4 cifras que se usará para generar el hash md5.
  md5.begin();                                                //Este número aleatorio debe ser distinto en cada petición, por ende también el hash.
  md5.add(salt+uid+station_password);
  md5.calculate();
  String hash = md5.toString();

  String url = "http://www.windguru.cz/upload/api.php?";

  url += "uid=" + uid;
  url += "&salt=" + salt;
  url += "&hash=" + hash;
  url += "&temperature=" + String(temperature,1);
  url += "&rh=" + String(humidity,1);

  if(wind_avg >= 0)                                           //Se puede colocar las variables de viento en -1 para que no las envíe, si la estación no tiene anemómetro
    url += "&wind_avg=" + String(wind_avg);
  if(wind_max >= 0)
    url += "&wind_max=" + String(wind_max);

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
