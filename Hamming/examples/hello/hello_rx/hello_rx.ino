/**
Example of the library using SoftwareSerial with a cheap 433Mhz modules.
You can change the message maximum length with Hamming::SIZE on hamming.cpp
*/
#include <hamming.h>

Hamming hamming(&Serial);
char message[Hamming::SIZE];

void setup()
{
  Serial.begin(2400);
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
