/**
Example of the library using SoftwareSerial with a cheap 433Mhz modules.
You can change the message maximum length with Hamming::SIZE on hamming.cpp
*/
#include <hamming.h>

Hamming hamming(&Serial);
char message[] = "Hello World!!";

void setup()
{
  Serial.begin(2400);
  delay(100);
}

void loop()
{
  hamming.write(message);
  delay(1000);
}
