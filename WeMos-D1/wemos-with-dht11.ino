// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
// Adafruit 有一系列的传感器,如温度湿度传感器DHT11/DHT22 https://github.com/adafruit/Adafruit_Sensor 
#include "DHT.h"
#define DHTPIN 3    // 定义从哪个针脚读取数据
#define DHTTYPE DHT11   // DHT 11 声明传感器类型
DHT dht(DHTPIN, DHTTYPE);
 
String key,value,times;
int relayPin = 16; // 保留用于继电器针脚,可以控制外部电源开关，针对在背面
const char* ssid = "upyun.com";
const char* password = "XXXXXXXX"; // wifi认证用户和密码
const String httpPost =   "POST / HTTP/1.1\r\n"
                          "Host: shaohy.17geek.us\r\n"
                          "User-Agent: Arduino/1.0\r\n"
                          "Content-Type: application/x-www-form-urlencoded;\r\n"
                          "\r\n"
                          "type=0&value=123\n";
//WiFiServer server(88);  // esp8266 不作服务端
 
void ledControl(int key,int value,int times){
  for(int i=0;i<times;i++) {
    digitalWrite(BUILTIN_LED, HIGH);  // turn on LED with voltage HIGH
    digitalWrite(relayPin,HIGH);
    delay(value);                      // wait one second
    digitalWrite(BUILTIN_LED, LOW);   // turn off LED with voltage LOW
    digitalWrite(relayPin,LOW);
    delay(value);                      // wait one second
  }
}
 
void setup() {
    Serial.begin(115200);
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {  
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");
    Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
    pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output,内置的LED与9同脚
    pinMode(relayPin,OUTPUT);
    // wemos作为客户端发送数据,不作服务器,因为一般用于内部ip
    //server.begin(); 
 
    // 初始化dht传感器
    dht.begin(); 
}
 
void loop() {
    delay(1000);  //Send a request every 1 sec, 每隔1s收发消息,初始化 DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
 
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
 
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("*C\t");
    Serial.print(f);
    Serial.print("*F\n");
 
    /*  第一种方式: 利用 web页面 + 定时刷新 获取数据*/
    if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
        HTTPClient http;
        http.begin("http://xx.xx.xx.xx/shaohy.php");
        int httpCode = http.POST(httpPost);
        String response = http.getString();
        Serial.println(httpCode);
        Serial.println(response);
        http.end();
 
        int commaIndex = response.indexOf(',');
        int secondCommaIndex = response.indexOf(',', commaIndex+1);
        key = response.substring(0, commaIndex);
        value = response.substring(commaIndex+1, secondCommaIndex);
        times = response.substring(secondCommaIndex+1);
    }
    else{
        Serial.println("Error in WiFi connection");
    }
 
    if( (key=="on") && (t>=28) ) {          // 从网页上读取数据,如果是启用,并且温度大于28c,就开始闪烁
        ledControl(key.toInt(),value.toInt(),times.toInt());
    }
}
