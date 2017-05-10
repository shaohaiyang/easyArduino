// ESP8266 库
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
 
String key,value,times;
int count = 0;
int relayPin = 16; // 保留用于继电器针脚,可以控制外部电源开关，针对在背面
const char* ssid = "upyun.com";
const char* password = "XXXXXXXXX"; // wifi认证用户和密码
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
    delay(value);                      // wait one second
    digitalWrite(BUILTIN_LED, LOW);   // turn off LED with voltage LOW
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
}
 
void loop() {
    /*  第一种方式: 利用 web页面 + 定时刷新 获取数据*/
    delay(3000);  //Send a request every 1 sec, 每隔1s收发消息,初始化 DHT11
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
 
    if( key=="on" ) {          // 从网页上读取数据,如果是启用,开始计数并循环
        ledControl(key.toInt(),value.toInt(),times.toInt());
        if(count >= 600){
          digitalWrite(relayPin,LOW);
          count=0;
        }
        else{
          digitalWrite(relayPin,HIGH);          
        }
        count++;
        Serial.println(count);
    }
    else{
          digitalWrite(relayPin,LOW);
          count=0;      
      }
}
