/*
  WireSlave.h - TWI/I2C Slave library for ESP32
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified December 2014 by Ivan Grokhotkov (ivan@esp8266.com) - esp8266 support
  Modified April 2015 by Hrsto Gochkov (ficeto@ficeto.com) - alternative esp8266 support
  Modified November 2017 by Chuck Todd <stickbreaker on GitHub>
    to use ISR and increase stability.
  Modified June 2020 by Gutierrez PS <gutierrezps on GitHub>
    as a workaround to temporarily provide I2C Slave functionality
*/

#ifndef TwoWireSlave_h
#define TwoWireSlave_h

#include <Arduino.h>
#include <driver/i2c.h>
#include <Stream.h>

#define I2C_BUFFER_LENGTH 128

class TwoWireSlave : public Stream
{
public:
    TwoWireSlave(uint8_t bus_num);
    ~TwoWireSlave();

    bool begin(int sda, int scl, int address);
    void update();

    size_t write(uint8_t);
    size_t write(const uint8_t *, size_t);
    int available(void);
    int read(void);
    int peek(void);
    void flush(void);

    inline size_t write(const char * s)
    {
        return write((uint8_t*) s, strlen(s));
    }
    inline size_t write(unsigned long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(long n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(unsigned int n)
    {
        return write((uint8_t)n);
    }
    inline size_t write(int n)
    {
        return write((uint8_t)n);
    }
    
    void onReceive(void (*)(int));
    void onRequest(void (*)());

private:
    uint8_t num;
    i2c_port_t portNum;
    int8_t sda;
    int8_t scl;

    uint8_t rxBuffer[I2C_BUFFER_LENGTH];
    uint16_t rxIndex;
    uint16_t rxLength;
    uint16_t rxQueued;

    uint8_t txBuffer[I2C_BUFFER_LENGTH];
    uint16_t txIndex;
    uint16_t txLength;
    uint16_t txAddress;
    uint16_t txQueued;

    void (*user_onRequest)(void);
    void (*user_onReceive)(int);
};


extern TwoWireSlave WireSlave;
extern TwoWireSlave WireSlave1;

#endif