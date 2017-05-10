// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
 
String key,num,value,times;
int oldnum=0,oldvol=0;
const char* ssid = "shaohy-home";
const char* password = "xxxxx"; // wifi认证用户和密码
const String httpPost =   "POST / HTTP/1.1\r\n"
                          "Host: shaohy.17geek.us\r\n"
                          "User-Agent: Arduino/1.0\r\n"
                          "Content-Type: application/x-www-form-urlencoded;\r\n"
                          "\r\n";
//WiFiServer server(88);  // esp8266 不作服务端

#define PIN_BUSY D5
SoftwareSerial mySoftwareSerial(D1, D2); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
 
void setup() {
// 初始化esp8266 wifi无线连接
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
 
  while (WiFi.status() != WL_CONNECTED) {  
      delay(500);
      Serial.print(".");
    }
  Serial.println(" connected");
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
 
// 初始化DFRobot Player 
  pinMode(PIN_BUSY, INPUT);
  mySoftwareSerial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
 
  delay(2000); // 等SD到位需要一点时间,这个延时很重要,否则初始化不成功 
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
}
 
void mp3play(int num,int value,int times){
  if(oldnum == num ){		// 如果是同一首歌
      if(digitalRead(PIN_BUSY) == HIGH){	// 已经停止播放了
        myDFPlayer.volume(value);  //Set volume value (0~30).
        myDFPlayer.play(num);  //Play the number key mp3
        Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
        }
      else{
        delay(1000);
        if(oldvol != value){
        myDFPlayer.volume(value);  //Set volume value (0~30).
        }
      }
  }
  else{
      myDFPlayer.volume(value);  //Set volume value (0~30).
      myDFPlayer.play(num);  //Play the number key mp3
      Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
      oldnum = num;
      oldvol = value;
    }  
}
 
void loop() {
    delay(1000);  //Send a request every 1 sec, 每隔1s收发消息
    /*  第一种方式: 利用 web页面 + 定时刷新 获取数据*/
    if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
        HTTPClient http;
        http.begin("http://xx.xx.xx.xx/shaohy2.php");
        int httpCode = http.POST(httpPost);
        String response = http.getString();
        Serial.println(response);
        http.end();

        int commaIndex = response.indexOf(',');
        int secondCommaIndex = response.indexOf(',', commaIndex+1);
        int thirdCommaIndex = response.indexOf(',',secondCommaIndex+1);
        key = response.substring(0, commaIndex);
        num = response.substring(commaIndex+1, secondCommaIndex);
        value = response.substring(secondCommaIndex+1,thirdCommaIndex);
        times = response.substring(thirdCommaIndex+1);
    }
    else{
        Serial.println("Error in WiFi connection");
    }
 
    if(key=="on") {          // 从网页上读取数据,如果是启用,就开始播放
        mp3play(num.toInt(),value.toInt(),times.toInt());
    }
    else{
        myDFPlayer.pause();  //pause the mp3
    }
}
