#include <math.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <SPI.h>
#include "epd1in54.h"
#include "epdpaint.h"
#include "HTTPSRedirect.h"
#include <dht11.h>
#include <NTPClient.h>

#define DHT11_PIN 4
#define COLORED     0
#define UNCOLORED   1
dht11 DHT11;
unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

//User Config
const char* ssid = "";
const char* password = "";
const char* GScriptId = "";
int updateTime=10;

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int con = 0, hour, minute, second, day, relay, timeSection, overTemp, over = 0, retrys = 0,refresh2=0,DZ=0;
const long utcOffsetInSeconds = -3600 * 7;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String line;
int num, firstConnect = 1, refresh = 1, TTemp,Ttemp, temp, Temp;
bool flag = false;
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url2 = String("/macros/s/") + GScriptId + "/exec";

HTTPSRedirect* client = nullptr;
unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  WiFi.begin(ssid, password);
  
  if (epd.Init(lut_full_update) != 0) {
    Serial.print("e-Paper init failed");
    return;
  }
  Serial.println("hi");
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  
  Serial.println("hi");
}

void timeStart() {
  if (WiFi.status() == WL_CONNECTED && con == 0) {
    timeClient.begin();
    con = 1;
    cal();
    timePoll();
    displayUpdate();
  }
}

void timePoll() {
  if (con == 1) {
    timeClient.update();
  }
}

void cal() {
  if (WiFi.status() == 3 && (!flag || refresh == 1)) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(false);
    client->setContentTypeHeader("application/json");
    int retval;
    if (WiFi.status() == 3) {
      retval = client->connect(host, httpsPort);
    }
    else {
      retval = 0;
    }
    if (retval == 1) {
      flag = true;
      client->GET(url2, host);
      
      line = client->getResponseBody();
      num = 0;
      if (line.length() >= 2) {
        for (int j = 0; j < (line.length() - 2); j++) {
          num += (line[j] - 48) * pow(10, line.length() - 3 - j);
        }
      }
      Serial.print("Pulled updated Target Temperature: ");
      Serial.println(num);
    }
    else {
      Serial.println("Connection failed. Retrying...");
      flag = false;
    }
    delete client;
    client = nullptr;
    retrys = 0;
  }
  else {
    retrys++;
    if (retrys >= 5) {
      num = 0;
    }
  }
  if (num == 0) {
    TTemp = 20;
  }
  else {
    TTemp = num;
  }
}

void displayUpdate() {
  if (timeClient.getMinutes() != minute || timeClient.getHours() != hour || timeClient.getDay() != day||Ttemp != TTemp|| temp !=Temp) {
    minute = timeClient.getMinutes(), hour = timeClient.getHours(), day = timeClient.getDay(),Ttemp=TTemp,Temp=temp;
    Serial.println("display updated");
    Serial.print("(");
    Serial.print(timeClient.getDay());
    Serial.print(",");
    Serial.print(timeClient.getHours());
    Serial.print(",");
    Serial.print(timeClient.getMinutes());
    Serial.print(",");
    Serial.print(timeClient.getSeconds());
    Serial.print(",");
    Serial.print(TTemp);
    Serial.print(",");
    Serial.print(temp);
    Serial.println(")");
    char time_string[] = { '0', '0', ':', '0', '0', '\0' };
    time_string[0] = hour / 10 + '0';
    time_string[1] = hour % 10 + '0';
    time_string[3] = minute / 10 + '0';
    time_string[4] = minute % 10 + '0';

    char Ttemp_string[] = { 'T', 'a', 'r', 'g', 'e', 't', ':', ' ', '0', '0', '\0' };
    Ttemp_string[8] = TTemp / 10 + '0';
    Ttemp_string[9] = TTemp % 10 + '0';

    char temp_string[] = { 'A', 'c', 't', 'u', 'a', 'l', ':', ' ', '0', '0', '\0' };
    temp_string[8] = Temp / 10 + '0';
    temp_string[9] = Temp % 10 + '0';

    paint.SetWidth(32);
    paint.SetHeight(134);
    paint.SetRotate(ROTATE_270);

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, daysOfTheWeek[day], &Font20, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 2, 62, paint.GetWidth(), paint.GetHeight());

    paint.SetWidth(32);
    paint.SetHeight(68);
    paint.SetRotate(ROTATE_270);

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, time_string, &Font20, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 4, 0, paint.GetWidth(), paint.GetHeight());

    paint.SetWidth(32);
    paint.SetHeight(166);
    paint.SetRotate(ROTATE_270);

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, Ttemp_string, &Font24, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 52, 30, paint.GetWidth(), paint.GetHeight());

    paint.SetWidth(32);
    paint.SetHeight(166);
    paint.SetRotate(ROTATE_270);

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 4, temp_string, &Font24, COLORED);
    epd.SetFrameMemory(paint.GetImage(), 100, 30, paint.GetWidth(), paint.GetHeight());
    epd.DisplayFrame();
  }
}

void loop() {
  DHT11.read(DHT11_PIN);
  temp=DHT11.temperature;
  timeStart();
  int tsec= timeClient.getMinutes()*60+timeClient.getSeconds();
  if(tsec%updateTime<=1&&refresh2==0){
    refresh2=1; 
  }
  if(refresh2==1){
    cal();
    timePoll();
    refresh2=0;
  }
  displayUpdate();
  if((TTemp-DZ/2)>=temp&&DZ=0){
    digitalWrite(15,HIGH);
  }else if(TTemp+DZ/2<=temp){
    digitalWrite(15,LOW);
  }
}
