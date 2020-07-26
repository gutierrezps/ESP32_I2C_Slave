/**
 * @file WirePacker.cpp
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief Class to pack the data before sending to other I2C device
 * @date 2020-06-16
 * 
 */

#include "WirePacker.h"
#include "WireCrc.h"

WirePacker::WirePacker()
{
    reset();
}

size_t WirePacker::write(uint8_t data)
{
    if (!isPacketOpen_) {
        return 0;
    }

    // leave room for crc and end bytes
    if (totalLength_ >= PACKER_BUFFER_LENGTH - 2) {
        return 0;
    }

    buffer_[index_] = data;
    ++index_;
    totalLength_ = index_;
    
    return 1;
}

size_t WirePacker::write(const uint8_t *data, size_t quantity)
{
    for (size_t i = 0; i < quantity; ++i) {
        if (!write(data[i])) {
            return i;
        }
    }
    return quantity;
}

void WirePacker::end()
{
    isPacketOpen_ = false;

    // make room for CRC byte
    ++index_;

    buffer_[index_] = frameEnd_;
    ++index_;
    totalLength_ = index_;

    buffer_[1] = totalLength_;

    // ignore start, length, crc and end bytes
    uint8_t payloadLength = totalLength_ - 4;

    WireCrc crc8;
    crc8.calc(&totalLength_, 1);   // include length in CRC
    uint8_t crc = crc8.update(buffer_ + 2, payloadLength);
    buffer_[index_-2] = crc;

    // prepare for reading
    index_ = 0;
}

size_t WirePacker::available()
{
    if (isPacketOpen_) {
        return 0;
    }

    return totalLength_ - index_;
}

int WirePacker::read()
{
    int value = -1;

    if (!isPacketOpen_ && index_ < totalLength_) {
        value = buffer_[index_];
        ++index_;
    }
    
    return value;
}

void WirePacker::reset()
{
    buffer_[0] = frameStart_;
    index_ = 2;
    totalLength_ = 2;
    isPacketOpen_ = true;
}


#ifdef PACKER_DEBUG

void WirePacker::printToSerial()
{
    Serial.printf("length: %d, ", totalLength_);

    if (isPacketOpen_) {
        Serial.print("open, ");
    }
    else {
        Serial.print("closed, ");
    }
    
    if (totalLength_ > 2) {
        Serial.print("buffer str: '");
        for (size_t i = 0; i < totalLength_; ++i) {
            if (buffer_[i] < 32 || buffer_[i] >= 127) {
                Serial.print(".");
            }
            else {
                Serial.print((char) buffer_[i]);
            }
        }
        Serial.print("', buffer hex: ");
        for (size_t i = 0; i < totalLength_; ++i) {
            Serial.printf("%02X ", buffer_[i]);
        }
    }
    else {
        Serial.print("empty");
    }

    Serial.println();
}

#endif      // ifdef PACKER_DEBUG
