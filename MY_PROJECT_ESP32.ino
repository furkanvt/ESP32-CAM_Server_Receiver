//==================================================================================
//          KÜTÜPHANE TANIMLAMALARI
//==================================================================================
#include <WiFi.h>
#include "esp_wifi.h"
#include <WebServer.h>
#include "OV2640.h"
#include "LineNotifyESP32.h"
#include <WiFiClientSecure.h>
//==================================================================================
//          WİFİ SSID VE PASSWORD
//==================================================================================
const char *ssid = "n4kruF";
const char *password = "88822666";
//==================================================================================
//          WİFİ AP MODU İÇİN SSID VE PASSWORD
//==================================================================================
const char *aPssid = "ESP32";
const char *aPpassword = "ESP32Test";
//==================================================================================
//          WİFİ SERVER AYARLARI
//==================================================================================
WebServer server(90);
WebServer server2(100);
//==================================================================================
//          KAMERA MODEL TANIMLAMA
//==================================================================================
#define CAMERA_MODEL_M5STACK_PSRAM

#if defined(CAMERA_MODEL_M5STACK_PSRAM)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    15
#define XCLK_GPIO_NUM     27
#define SIOD_GPIO_NUM     25
#define SIOC_GPIO_NUM     23

#define Y9_GPIO_NUM       19
#define Y8_GPIO_NUM       36
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM        5
#define Y4_GPIO_NUM       34
#define Y3_GPIO_NUM       35
#define Y2_GPIO_NUM       17
#define VSYNC_GPIO_NUM    22
#define HREF_GPIO_NUM     26
#define PCLK_GPIO_NUM     21

#endif

HTTPClientESP32Ex http;
OV2640 cam;
//==================================================================================
//          LINE MESAJ İÇİN TANIMLAMALAR
//==================================================================================
String LINE_TOKEN = "BycSzifdRdq8znBVIo3ZJFZ94WejWF8KqRFRL157f72";
//==================================================================================
//          PROGRAM SETUP BÖLÜMÜ
//==================================================================================
void setup(){
  Serial.begin(115200);
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);
  WiFiBaglantiSetup();
  KameraSetup();
}
//==================================================================================
//          PROGRAM LOOP BÖLÜMÜ
//==================================================================================
void loop(){
  if(WiFi.status() != WL_CONNECTED)
  {
     WiFiBaglantiSetup();
  }
  wifi_sta_list_t stationList;
  esp_wifi_ap_get_sta_list(&stationList); 
  Serial.print("Bağlı Cihaz Sayısı: ");
  Serial.println(stationList.num);
  delay(3000);
  server.handleClient();
  server2.handleClient();
}
//==================================================================================
//          KAMERA SETUP AYARLARI
//==================================================================================
void KameraSetup()
{
  camera_config_t camera_config;
  camera_config.ledc_channel = LEDC_CHANNEL_0;
  camera_config.ledc_timer = LEDC_TIMER_0;
  camera_config.pin_d0 = 17;
  camera_config.pin_d1 = 35;
  camera_config.pin_d2 = 34;
  camera_config.pin_d3 = 5;
  camera_config.pin_d4 = 39;
  camera_config.pin_d5 = 18;
  camera_config.pin_d6 = 36;
  camera_config.pin_d7 = 19;
  camera_config.pin_xclk = 27;
  camera_config.pin_pclk = 21;
  camera_config.pin_vsync = 22;
  camera_config.pin_href = 26;
  camera_config.pin_sscb_sda = 25;
  camera_config.pin_sscb_scl = 23;
  camera_config.pin_reset = 15;
  camera_config.xclk_freq_hz = 20000000;
  camera_config.pixel_format = CAMERA_PF_JPEG;
  camera_config.frame_size = CAMERA_FS_SVGA;

  cam.init(camera_config);

  delay(100);

  cam.run();
  
}
//==================================================================================
//          SERVER SETUP AND AP-STA MODE CONFIGRATION
//==================================================================================
void WiFiBaglantiSetup()
{
  delay(1000);
  
  Serial.println();
  Serial.println();
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Bağlanıyor ");
  Serial.println(ssid);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(16, HIGH);
    delay(50);
    digitalWrite(16, LOW);
    delay(50);
  }
   //digitalWrite(16, HIGH);
   Serial.println();
   Serial.println("WiFi Bağlantısı Kuruldu ");
   Serial.print("IP Adresi: ");
   Serial.println(WiFi.localIP());

   Serial.print("Wifi Erişim Noktasını Yapılandırma...\n ");
   WiFi.softAP(aPssid,aPpassword);
   IPAddress softIP = WiFi.softAPIP();

   Serial.println("Erişim Noktası Başarılı! ");
   Serial.print("Soft IP Adresi: ");
   Serial.println(softIP);

  server2.on("/guvenlik/", HTTP_GET, guvenlikDataOku);
  server.on ("/sensor/", HTTP_GET, dataOku);
  server.begin();
  server2.begin();
}
//==================================================================================
//          SERVERİN GELEN DATAYI OKUMASI
//==================================================================================
void dataOku()
{
  if ( server.hasArg("sensor_oku") )
  {
     int okunanDeger = server.arg("sensor_oku").toInt();
     Serial.print("Gelen Veri: ");
     Serial.println(okunanDeger);
     if ( okunanDeger == 1) {
         Serial.println("Hareket Algılandı!!!!");
         Line_Notify("Hareket Algılandı!!!");
         Serial.println("Görüntüler Gönderiliyor...");
         Line_Notify("Görüntüler Gönderiliyor...!!!");
         /*for(int i = 0; i < 3 ; i++)
         {
          Line_Notify_Image();
          delay(3000);
         }*/
         Line_Notify_Image();
     }
     server.send(200, "text/html", "Data Alındı");
  }
}
//==================================================================================
//          SERVER GÜVENLİK BAŞLATMA BİLGİSİ
//==================================================================================
void guvenlikDataOku()
{
  if ( server2.hasArg("guvenlik_oku") )
  {
     int okunanDeger = server2.arg("guvenlik_oku").toInt();
     Serial.print("Guvenlik Bilgi: ");
     Serial.println(okunanDeger);
     if ( okunanDeger == 1){
         for(int i = 0; i<5 ; i++){
           digitalWrite(16, HIGH);
           delay(50);
           digitalWrite(16, LOW);
           delay(50);
         }           
         Serial.println("GÜVENLİK BAŞLATILDI!!!!");
         Line_Notify("GÜVENLİK BAŞLATILDI.");
         delay(2000);
     }
     else {
         Serial.println("GÜVENLİK SONLANDIRILDI!!!");
         Line_Notify("GÜVENLİK SONLANDIRILDI.");
         delay(2000);
     }
     server2.send(200, "text/html", "GÜVENLİK BAŞLATMA BİLGİSİ ALINDI...");
  } 
}
//==================================================================================
//          LINE MESAJ GÖNDERME
//==================================================================================
void Line_Notify(String mesaj) {
  
  WiFiClientSecure client;
  
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Bağlantı Hatası");
    delay(2000);
    return;
  }

  String istek = "";
  istek += "POST /api/notify HTTP/1.1\r\n";
  istek += "Host: notify-api.line.me\r\n";
  istek += "Authorization: Bearer " + LINE_TOKEN + "\r\n";
  istek += "Cache-Control: no-cache\r\n";
  istek += "User-Agent: ESP32\r\n";
  istek += "Content-Type: application/x-www-form-urlencoded\r\n";
  istek += "Content-Length: " + String(String("message=" + mesaj).length()) + "\r\n";
  istek += "\r\n";
  istek += "message=" + mesaj;
  Serial.println(istek);
  client.print(istek);

  delay(20);

  Serial.println("-------------");
  while (client.connected()) {
    String satir = client.readStringUntil('\n');
    if (satir == "\r") {
      break;
    }
    Serial.println(satir);
  }
  Serial.println("-------------");
}
//==================================================================================
//          LİNE GÖRÜNTÜ GÖNDERME
//==================================================================================
void Line_Notify_Image()
{
  lineNotify.init(LINE_TOKEN);

  uint8_t durum = lineNotify.sendLineImageData(http, "Görüntü" , "image.jpg", cam.getfb(), cam.getSize());
  if (durum == LineNotifyESP32::LineStatus::SENT_COMPLETED)
  {
    Serial.println("Kameradan görüntü gönderimi tamamlandı.");
    Serial.println("Mesaj Sınırı: " + String(lineNotify.textMessageLimit()));
    Serial.println("Kalan Mesaj Sınırı: " + String(lineNotify.textMessageRemaining()));
    Serial.println("Görüntü Mesaj Sınırı: " + String(lineNotify.imageMessageLimit()));
    Serial.println("Kalan Görüntü Mesaj Sınırı: " + String(lineNotify.imageMessageRemaining()));
  }
  else if (durum == LineNotifyESP32::LineStatus::SENT_FAILED)
    Serial.println("Kameradan görüntü gönderilemedi!");
  else if (durum == LineNotifyESP32::LineStatus::CONNECTION_FAILED)
    Serial.println("LINE servisine bağlantı başarısız!");
  Serial.println();
}
