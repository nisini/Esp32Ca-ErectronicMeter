#include <WiFi.h>
const char* ssid = "";
const char* password = "";

void wifiSetup(){
  WiFi.mode(WIFI_STA);
  Serial.printf("connecting to %s¥n", ssid);
  WiFi.begin(ssid, password);
  int wifiFalseCount = 0;
  int rebootCount = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (10 < wifiFalseCount) {
      WiFi.disconnect(true, true) ;
      WiFi.begin(ssid, password);
      if (3 < rebootCount) {
        ESP.restart() ;
      }
    }
    wifiFalseCount++;
    Serial.printf("connecting to %s¥n", ssid);
    Serial.print(".");
    delay(500);
  }


  Serial.println();
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.localIP());
}
void checkWifi(){
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect(true, true);
    wifiSetup();
  }
}
