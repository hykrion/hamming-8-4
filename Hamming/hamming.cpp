/*
  hamming.cpp - hamming(8,4) ECC library for Arduino.
  Copyright (c) 2016 José M Sirvent.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "hamming.h"

static const uint8_t m_tx[] PROGMEM = {0x00, 0x1e, 0x2d, 0x33, 0x4b, 0x55, 0x66, 0x78, 0x87, 0x99, 0xaa, 0xb4, 0xcc, 0xd2, 0xe1, 0xff};
static const uint8_t m_rx[] PROGMEM = {
  0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x10, 0x08, 0x00, 0x10, 0x10, 0x04, 0x10, 0x02, 0x01, 0x10,
  0x00, 0x10, 0x10, 0x03, 0x10, 0x05, 0x01, 0x10, 0x10, 0x09, 0x01, 0x10, 0x01, 0x10, 0x01, 0x01,
  0x00, 0x10, 0x10, 0x03, 0x10, 0x02, 0x06, 0x10, 0x10, 0x02, 0x0a, 0x10, 0x02, 0x02, 0x10, 0x02,
  0x10, 0x03, 0x03, 0x03, 0x0b, 0x10, 0x10, 0x03, 0x07, 0x10, 0x10, 0x03, 0x10, 0x02, 0x01, 0x10,
  0x00, 0x10, 0x10, 0x04, 0x10, 0x05, 0x06, 0x10, 0x10, 0x04, 0x04, 0x04, 0x0c, 0x10, 0x10, 0x04,
  0x10, 0x05, 0x0d, 0x10, 0x05, 0x05, 0x10, 0x05, 0x07, 0x10, 0x10, 0x04, 0x10, 0x05, 0x01, 0x10,
  0x10, 0x0e, 0x06, 0x10, 0x06, 0x10, 0x06, 0x06, 0x07, 0x10, 0x10, 0x04, 0x10, 0x02, 0x06, 0x10,
  0x07, 0x10, 0x10, 0x03, 0x10, 0x05, 0x06, 0x10, 0x07, 0x07, 0x07, 0x10, 0x07, 0x10, 0x10, 0x0f,
  0x00, 0x10, 0x10, 0x08, 0x10, 0x08, 0x08, 0x08, 0x10, 0x09, 0x0a, 0x10, 0x0c, 0x10, 0x10, 0x08,
  0x10, 0x09, 0x0d, 0x10, 0x0b, 0x10, 0x10, 0x08, 0x09, 0x09, 0x10, 0x09, 0x10, 0x09, 0x01, 0x10,
  0x10, 0x0e, 0x0a, 0x10, 0x0b, 0x10, 0x10, 0x08, 0x0a, 0x10, 0x0a, 0x0a, 0x10, 0x02, 0x0a, 0x10,
  0x0b, 0x10, 0x10, 0x03, 0x0b, 0x0b, 0x0b, 0x10, 0x10, 0x09, 0x0a, 0x10, 0x0b, 0x10, 0x10, 0x0f,
  0x10, 0x0e, 0x0d, 0x10, 0x0c, 0x10, 0x10, 0x08, 0x0c, 0x10, 0x10, 0x04, 0x0c, 0x0c, 0x0c, 0x10,
  0x0d, 0x10, 0x0d, 0x0d, 0x10, 0x05, 0x0d, 0x10, 0x10, 0x09, 0x0d, 0x10, 0x0c, 0x10, 0x10, 0x0f,
  0x0e, 0x0e, 0x10, 0x0e, 0x10, 0x0e, 0x06, 0x10, 0x10, 0x0e, 0x0a, 0x10, 0x0c, 0x10, 0x10, 0x0f,
  0x10, 0x0e, 0x0d, 0x10, 0x0b, 0x10, 0x10, 0x0f, 0x07, 0x10, 0x10, 0x0f, 0x10, 0x0f, 0x0f, 0x0f
};

Hamming::Hamming(Stream* stream)
{
  clearMessage();
  m_rHigh = 0;
  m_rLow = 0;
  m_receivingMessage = false;
  m_messageReady = false;
  m_stream = stream;
}

Hamming::~Hamming()
{
}

void Hamming::clearMessage()
{
  memset(m_message, '\0', SIZE);
  m_messageIndex = 0;
  m_messageReady = false;
}

void Hamming::tx(uint8_t c)
{
  uint8_t txHigh, txLow;

  txHigh = (c >> 4) & 0X0F;
  txLow = c & 0x0F;

  m_stream->write(pgm_read_byte(m_tx + txHigh));
  m_stream->write(pgm_read_byte(m_tx + txLow));
}

uint8_t Hamming::rx(uint8_t data)
{
  return pgm_read_byte(m_rx + data);
}

void Hamming::write(const char* message)
{
  tx(m_messageBegin);
  uint8_t i = 0;
  while(message[i] != '\0' && i < SIZE)
  {
    tx(message[i]);
    i++;
  }
  tx(m_messageEnd);
  
  m_stream->flush();
}

void Hamming::write(const uint8_t b)
{
  write((const uint8_t*)&b, sizeof(uint8_t));
}

void Hamming::write(const int i)
{
  write((const uint8_t*)&i, sizeof(int));
}

void Hamming::write(const float f)
{
  write((const uint8_t*)&f, sizeof(float));
}

void Hamming::write(const double d)
{
  write((const uint8_t*)&d, sizeof(double));
}

void Hamming::write(const void* data, uint8_t size)
{
  uint8_t* p = (uint8_t*)data;
  
  tx(m_messageBegin);
  for (uint8_t j = 0; j < size; j++)
  {
    tx(*p);
    p++;
  }
  tx(m_messageEnd);

  m_stream->flush();
}

uint8_t Hamming::available()
{
  static uint8_t counter = 0;
  
  while(m_stream->available())
  {
    uint8_t c = m_stream->read();

    counter = (counter + 1) % 2;
    
    if(counter == 1)
      m_rHigh = rx(c);
    else if (counter == 0)
    {
      m_rLow = rx(c);
      
      if(m_rHigh != m_messageError && m_rLow != m_messageError)
      {
        uint8_t r = ((m_rHigh << 4) & 0xF0) | m_rLow;
        
        if(m_receivingMessage)
        {
          if(r != m_messageEnd)
          {
            m_message[m_messageIndex] = r;
            m_messageIndex = (m_messageIndex + 1) % SIZE;
          }
          else
          {
            m_receivingMessage = false;
            m_messageReady = true;
          }
        }
        else if(r == m_messageBegin)
        {
          m_receivingMessage = true;
        }
      }
    }
  }

  return m_messageReady;
}

void Hamming::read(char* message)
{
  strcpy(message, (char*)m_message);
  clearMessage();
}

void Hamming::read(uint8_t& b)
{
  char buffer[SIZE];
  read(buffer);

  b = (uint8_t)buffer[0];
}

void Hamming::read(int& i)
{
  typedef union {
    int theInt;
    uint8_t theByte[2];
  } byteInt;
  
  byteInt foo;
  foo.theByte[1] = m_message[1];
  foo.theByte[0] = m_message[0];

  i = foo.theInt;
  clearMessage();
}

void Hamming::read(float& f)
{
  typedef union {
    float theFloat;
    uint8_t theByte[4];
  } byteFloat;
  
  byteFloat foo;
  foo.theByte[3] = m_message[3];
  foo.theByte[2] = m_message[2];
  foo.theByte[1] = m_message[1];
  foo.theByte[0] = m_message[0];

  f = foo.theFloat;
  clearMessage();
}

void Hamming::read(double& d)
{
  typedef union {
    float theDouble;
    uint8_t theByte[4];
  } byteDouble;
  
  byteDouble foo;
  foo.theByte[3] = m_message[3];
  foo.theByte[2] = m_message[2];
  foo.theByte[1] = m_message[1];
  foo.theByte[0] = m_message[0];

  d = foo.theDouble;
  clearMessage();
}