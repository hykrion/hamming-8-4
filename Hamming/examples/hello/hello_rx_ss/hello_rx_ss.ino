/**
Example of the library using SoftwareSerial with a cheap 433Mhz modules.
You can change the message maximum length with Hamming::SIZE on hamming.cpp
*/
#include <hamming.h>
#include <SoftwareSerial.h>

#define RX 10
#define TX 11

SoftwareSerial ss(RX, TX);

Hamming hamming(&ss);
char message[Hamming::SIZE];

void setup()
{
  Serial.begin(9600);
  delay(100);
  ss.begin(2400);
  delay(100);
}

void loop()
{
  if(hamming.available())
  {
    hamming.read(message);
    Serial.println(message);
  }
}
