#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <Preferences.h>//今回は使えませんでした。
//#include <SD.h>

WebServer server(80);
Preferences preferences;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  

  /*
  preferences.begin("my-app", true);
  int exposure = preferences.getInt("exposure", 1);
  int aec = preferences.getInt("aec", 1);
  int ae_level = preferences.getInt("ae_level", 1);
  int effect = preferences.getInt("effect", 0);
  int brightness = preferences.getInt("brightness", 0);
  int contrast=preferences.getInt("contrast", 0);
  int wb_mode=preferences.getInt("wb_mode", 0);
  preferences.end();
  */

  initCamera();
  wifiSetup();
　//http://host/capture で呼ばれます。
  server.on("/capture", []
            { onCapture(false); });
 
  server.on("/capture/flash", []
            { onCapture(true); });
  //今回は使いません
  server.on("/setting", onSetting);

  server.begin();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}
void onSetting()
{
  if (server.method() == HTTP_POST)
  {
  if(preferences.begin("my-app", false)){
    Serial.println("OK");
  }else{
    Serial.println("false");
  }
    unsigned int flash_brightness = server.arg("flash_brightness").toInt();
    preferences.putUInt("flash_brightness",flash_brightness);
    preferences.end();
    Serial.print("flash_brightness:");
    Serial.println(flash_brightness);
    server.send(200, "text/plain", "esp complete");
  }
}
void onCapture(bool flash)
{
  //写真をとります。
  camera_fb_t *fb = capture(flash,10);
  //写真が正常か確認します。
  if (crash_camera_fb_t(fb))
  {
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    server.send(400, "text/plain", "capture faild");
    ESP.restart();
  }
  server.send(200, "text/plain", "this works as well");
  postingImage(fb);//写真を送ります。
  esp_camera_fb_return(fb);//bufferの開放？
}
void postingImage(camera_fb_t *fb)
{
  HTTPClient client;
  client.begin("http://192.168.1.117:8888/imageUpdate");//ipアドレスは各自置き換えてね
  client.addHeader("Content-Type", "image/jpeg");
  int httpResponseCode = client.POST(fb->buf, fb->len);//これで送ります。
  if (httpResponseCode == 200)
  {
    Serial.println("成功200");
  }
  else
  {
    Serial.println("node.jsから応答がありません");
  }
  client.end();
}
void loop()
{
  // put your main code here, to run repeatedly:
  server.handleClient();
  checkWifi();
  delay(1000);
}
