// Arduino MQTT Client 库  https://github.com/knolleary/pubsubclient 
#include <PubSubClient.h>
 
// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
String key,value,times;
String response;
int relayPin = 0; // 保留用于继电器针脚,可以控制外部电源开关，针对在背面
int ledPin = 2;
 
WiFiClient espClient;
PubSubClient client(espClient);
 
const char* ssid = "Upyun-Guest";
const char* password = "XXXX"; // wifi认证用户和密码
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
 
void callback(char* topic, byte* payload, unsigned int length) {
    response="";
    Serial.print("-> Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      response+=(char)payload[i];      
    }
    Serial.println();
 
    int commaIndex = response.indexOf(',');
    int secondCommaIndex = response.indexOf(',', commaIndex+1);
    int thirdCommaIndex = response.indexOf(',',secondCommaIndex+1);
    key = response.substring(0, commaIndex);
    value = response.substring(commaIndex+1, secondCommaIndex);
    times = response.substring(secondCommaIndex+1,thirdCommaIndex);
 
    if( key=="on" ) {          // 从网页上读取数据,如果是启用,开始计数并循环
        ledControl(key.toInt(),value.toInt(),times.toInt());
        digitalWrite(relayPin,HIGH);          
        delay(1000*(value.toInt()));
        digitalWrite(relayPin,LOW);
    }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("-> Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic",clientId.c_str());
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      delay(1000);
    }
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
 
  pinMode(relayPin,OUTPUT);
  pinMode(ledPin,OUTPUT);
 
  // 初始化Arduino MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
 
void loop() {
    /* MQTT 初始化连接*/
    if (!client.connected()) {
       reconnect();
    }
    client.loop();
//    client.publish("outTopic","hello arduino");
}
