#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

#define PIN_BUSY D5

SoftwareSerial mp3Serial(D1, D2); // RX, TX

void setup () {
  pinMode(PIN_BUSY, INPUT);
  Serial.begin (115200);
  Serial.println("Setting up software serial");

  mp3Serial.begin (9600);
  Serial.println("Setting up mp3 player");
  mp3_set_serial (mp3Serial);  

  // Delay is required before accessing player. From my experience it's ~1 sec
  delay(1000); 
  mp3_set_volume (10);
}

void loop () {
  if(digitalRead(PIN_BUSY) == HIGH){
    delay(1000);
    Serial.println("play next");
    mp3_next ();  //Play next mp3 every 3 second.
  }
  else{
    Serial.print("Busy: ");
    delay(500);
  }
}
