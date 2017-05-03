// Arduino MQTT Client 库  https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>
 
// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
String key,value,times;
String response;
int relayPin = 0; // ESP-01 的GPIO,默认空脚为高电平,所以,传感器必须要以低电平触发才能正常
int ledPin = 2; // ESP-01 只有两个GPIO,好好利用一下
 
WiFiClient espClient;
PubSubClient client(espClient);
 
const char* ssid = "Upyun-Guest";
const char* password = "xxxxx"; // wifi认证用户和密码
const char* mqtt_server = "xx.xx.xx.xx";
const int mqtt_port = 1883;
const String httpPost =   "POST / HTTP/1.1\r\n"
                          "Host: shaohy.17geek.us\r\n"
                          "User-Agent: Arduino/1.0\r\n"
                          "Content-Type: application/x-www-form-urlencoded;\r\n"
                          "\r\n";
//WiFiServer server(88);  // esp8266 不作服务端
 
void ledControl(int value,int times){
  for(int i=0;i<times;i++) {
    digitalWrite(ledPin,HIGH);
    delay(value);                      // wait one second
    digitalWrite(ledPin,LOW);
    delay(value);                      // wait one second
  }
}
 
void setup()
{
  Serial.begin(115200);
  // 初始化esp8266 wifi无线连接
  WiFi.mode(WIFI_STA);
  delay(100);
 
  Serial.printf("Connecting to %s ", ssid); // 连接公司的无线
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
  Serial.println(" connected");
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
  pinMode(relayPin,INPUT);
  pinMode(ledPin,OUTPUT);
 
  // 初始化Arduino MQTT
  client.setServer(mqtt_server, mqtt_port);
}
 
void loop() {
  delay(500);
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status  
    if (!client.connected()){ // 如果没有连接,就先初始化连接
    Serial.print("-> Attempting MQTT connection...");
    String clientId = "ESP8266v31-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      client.publish("outTopic",clientId.c_str()); // 连接成功后,推送信息
        if(digitalRead(relayPin) == LOW){ //默认空脚为高电平,所以,传感器必须要以低电平触发才能正常
          ledControl(100,3); // 先给个发送灯
          client.publish("inTopic","on,60,10");
          ledControl(500,3); // 再给个结束灯
        }
      }
    }
    else{ // 已经建立连接,就可以判 LOW 断是否触摸
      if(digitalRead(relayPin) == LOW){ //默认空脚为高电平,所以,传感器必须要以低电平触发才能正常
          ledControl(100,3);
          client.publish("inTopic","on,60,10");
          ledControl(500,3);
        }
      }
  }
}
