/**
 * @file WireSlave.cpp
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief TWI/I2C slave library for ESP32 based on ESP-IDF slave API
 * @date 2021-02-21
 *
 */
#ifdef ARDUINO_ARCH_ESP32
#include <Arduino.h>
#include <driver/i2c.h>

#include "WireSlave.h"

TwoWireSlave::TwoWireSlave(uint8_t bus_num)
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
    ,packer_()
    ,unpacker_()
{

}

TwoWireSlave::~TwoWireSlave()
{
    flush();
    i2c_driver_delete(portNum);
}


bool TwoWireSlave::begin(int sda, int scl, int address)
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

void TwoWireSlave::update()
{
    uint8_t inputBuffer[I2C_BUFFER_LENGTH] = {0};
    int16_t inputLen = 0;

    inputLen = i2c_slave_read_buffer(portNum, inputBuffer, I2C_BUFFER_LENGTH, 1);

    if (inputLen <= 0) {
        // nothing received or error
        return;
    }

    if (!unpacker_.isPacketOpen()) {
        // start unpacking
        unpacker_.reset();
    }

    unpacker_.write(inputBuffer, size_t(inputLen));

    if (unpacker_.isPacketOpen() || unpacker_.totalLength() == 0) {
        // still waiting bytes,
        // or received bytes that are not inside a packet
        return;
    }

    if (unpacker_.hasError()) {
        return;
    }

    if (unpacker_.available()) {
        rxIndex = 0;
        rxLength = unpacker_.available();

        // transfer bytes from packet to rxBuffer
        while (unpacker_.available()) {
            rxBuffer[rxIndex] = unpacker_.read();
            rxIndex++;
        }
        rxIndex = 0;

        // call user callback
        if (user_onReceive) {
            user_onReceive(rxLength);
        }
    }
    else if (user_onRequest) {
        txIndex = 0;
        txLength = 0;
        packer_.reset();
        user_onRequest();
        packer_.end();

        while (packer_.available()) {
            txBuffer[txIndex] = packer_.read();
            ++txIndex;
        }
        txLength = txIndex;

        i2c_reset_tx_fifo(portNum);
        i2c_slave_write_buffer(portNum, txBuffer, txLength, 0);
    }
}

size_t TwoWireSlave::write(uint8_t data)
{
    if (packer_.packetLength() >= I2C_BUFFER_LENGTH) {
        return 0;
    }

    return packer_.write(data);
}

size_t TwoWireSlave::write(const uint8_t *data, size_t quantity)
{
    for (size_t i = 0; i < quantity; ++i) {
        if (!write(data[i])) {
            return i;
        }
    }
    return quantity;
}

int TwoWireSlave::available(void)
{
    return rxLength - rxIndex;
}

int TwoWireSlave::read(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
        ++rxIndex;
    }
    return value;
}

int TwoWireSlave::peek(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
    }
    return value;
}

void TwoWireSlave::flush(void)
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

void TwoWireSlave::onReceive(void (*function)(int))
{
    user_onReceive = function;
}

void TwoWireSlave::onRequest(void (*function)(void))
{
    user_onRequest = function;
}

TwoWireSlave WireSlave = TwoWireSlave(0);
TwoWireSlave WireSlave1 = TwoWireSlave(1);

#endif      // ifdef ARDUINO_ARCH_ESP32
