/*
  SlaveTwoWire.cpp - TWI/I2C Slave library for ESP32
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

#include <Arduino.h>
#include <driver/i2c.h>

#include "SWire.h"

SlaveTwoWire::SlaveTwoWire(uint8_t bus_num)
    :num(bus_num & 1)
    ,portNum(i2c_port_t(bus_num & 1))
    ,sda(-1)
    ,scl(-1)
    ,rxIndex(0)
    ,rxLength(0)
    ,rxQueued(0)
    ,txIndex(0)
    ,txLength(0)
    ,txAddress(0)
    ,txQueued(0)
{}

SlaveTwoWire::~SlaveTwoWire()
{
    flush();
    i2c_driver_delete(portNum);
}


bool SlaveTwoWire::begin(int sda, int scl, int address)
{
    i2c_config_t config;
    config.sda_io_num = gpio_num_t(sda);
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = gpio_num_t(scl);
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.mode = I2C_MODE_SLAVE;
    config.slave.addr_10bit_en = 0;
    config.slave.slave_addr = address & 0x7F;

    esp_err_t res = i2c_param_config(portNum, &config);

    if (res != ESP_OK) {
        log_e("invalid I2C parameters");
        return false;
    }

    res = i2c_driver_install(
            portNum,
            config.mode,
            2 * I2C_BUFFER_LENGTH,  // rx buffer length
            2 * I2C_BUFFER_LENGTH,  // tx buffer length
            0);

    if (res != ESP_OK) {
        log_e("failed to install I2C driver");
    }
    return res == ESP_OK;
}

void SlaveTwoWire::update()
{
    rxLength = i2c_slave_read_buffer(portNum, rxBuffer, I2C_BUFFER_LENGTH, 0);
    
    if (rxLength == 0) {
        return;
    }

    rxIndex = 0;
    if (user_onReceive) user_onReceive(rxLength);

    if (user_onRequest) {
        txIndex = 0;
        txLength = 0;
        user_onRequest();
        i2c_reset_tx_fifo(portNum);
        i2c_slave_write_buffer(portNum, txBuffer, txLength, 0);
    }
}

size_t SlaveTwoWire::write(uint8_t data)
{
    if (txLength >= I2C_BUFFER_LENGTH) {
        return 0;
    }

    txBuffer[txIndex] = data;
    ++txIndex;
    txLength = txIndex;
    return 1;
}

size_t SlaveTwoWire::write(const uint8_t *data, size_t quantity)
{
    for (size_t i = 0; i < quantity; ++i) {
        if (!write(data[i])) {
            return i;
        }
    }
    return quantity;
}

int SlaveTwoWire::available(void)
{
    return rxLength - rxIndex;
}

int SlaveTwoWire::read(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
        ++rxIndex;
    }
    return value;
}

int SlaveTwoWire::peek(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
    }
    return value;
}

void SlaveTwoWire::flush(void)
{
    rxIndex = 0;
    rxLength = 0;
    txIndex = 0;
    txLength = 0;
    rxQueued = 0;
    txQueued = 0;
    i2c_reset_rx_fifo(portNum);
    i2c_reset_tx_fifo(portNum);
}

void SlaveTwoWire::onReceive(void (*function)(int))
{
    user_onReceive = function;
}

void SlaveTwoWire::onRequest(void (*function)(void))
{
    user_onRequest = function;
}

SlaveTwoWire SWire = SlaveTwoWire(0);
SlaveTwoWire SWire1 = SlaveTwoWire(1);