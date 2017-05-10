// Arduino MQTT Client 库  https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>

// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

String key,value,times;
String response;
int relayPin = D0; // 保留用于继电器针脚,可以控制外部电源开关，针对在背面
int ledPin = 2;

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "upyun.com";
const char* password = "xxxxx"; // wifi认证用户和密码
const char* ussid = "NodeMCU";
const char* upassword = "yyyyyy";
const char* mqtt_server = "xx.xx.xx.xx";
const int mqtt_port = 1883;
const String httpPost =   "POST / HTTP/1.1\r\n"
                          "Host: shaohy.17geek.us\r\n"
                          "User-Agent: Arduino/1.0\r\n"
                          "Content-Type: application/x-www-form-urlencoded;\r\n"
                          "\r\n";
//WiFiServer server(88);  // esp8266 不作服务端

void ledControl(int key,int value,int times){
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
  WiFi.mode(WIFI_AP_STA);
  delay(100);

  Serial.printf("Connecting to %s ", ssid); // 连接公司的无线
  WiFi.begin(ssid, password);
  Serial.print("Setting soft-AP ... ");   // 创建自己的WPA2-PSK 无线网络
  Serial.println(WiFi.softAP(ussid, upassword)?"Ready":"Failed!");

  while (WiFi.status() != WL_CONNECTED) {
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
  if(digitalRead(relayPin) == HIGH){
      if (!client.connected()){
        Serial.print("-> Attempting MQTT connection...");
        String clientId = "NodeMCU-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
          Serial.println("connected");
          client.publish("inTopic","on,180,10");
          }
        }
        else{
          Serial.println("-> Already connect MQTT.");
          client.publish("inTopic","on,180,10");
        }
  }
}
