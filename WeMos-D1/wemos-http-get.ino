#include <ESP8266WiFi.h>
const char* ssid = "shaohy-home";
const char* password = "xxxxxxxxx";
const char* host = "xx.xx.xx.xx";
int port = 80;
int potPin = 0;
int g_speed = 100;
 
void setup() {
  Serial.begin(115200);
  Serial.println();
 
  WiFi.begin(ssid,password);
 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  g_speed = analogRead(potPin);
 
  WiFiClient client;
  Serial.printf("\n===============\n\t%d\n===============\n",g_speed);
  Serial.printf("\n[Connecting to %s ... ", host);
  if (client.connect(host, port)) {
    Serial.println("connected]");
    Serial.println("[Sending a request]");
    client.print(String("GET /shaohy.php?speed=") + g_speed + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
 
    Serial.println("[Response:]");
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
  }
  else {
    Serial.println("connection failed!]");
    client.stop();
  }
  delay(3000);  
}
