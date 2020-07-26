/**
 * @file WireUnpacker.cpp
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief Class to unpack the data packed with WirePacker
 * @date 2020-06-16
 * 
 */
#include "WireUnpacker.h"
#include "WireCrc.h"

WireUnpacker::WireUnpacker()
    :index_(0)
    ,totalLength_(0)
    ,payloadLength_(0)
    ,isPacketOpen_(false)
    ,expectedLength_(0)
    ,expectedCrc_(0)
    ,lastError_(WireUnpacker::NONE)
{
}

size_t WireUnpacker::write(uint8_t data)
{
    if (totalLength_ >= UNPACKER_BUFFER_LENGTH || hasError()) {
        return 0;
    }

    if (!isPacketOpen_) {
        // enable writing only if buffer is empty
        if (totalLength_ == 0 && data == frameStart_) {
            isPacketOpen_ = true;
            ++totalLength_;
            return 1;
        }
        return 0;
    }

    // first byte after start is packet length
    if (expectedLength_ == 0) {
        expectedLength_ = data;

        if (expectedLength_ > UNPACKER_BUFFER_LENGTH) {
            isPacketOpen_ = false;
            lastError_ = INVALID_LENGTH;
            return 0;
        }

        ++totalLength_;
        return 1;
    }

    // if end byte index wasn't reached
    if (totalLength_ < (expectedLength_ - 1)) {
        buffer_[index_] = data;
        ++index_;
        ++totalLength_;
        return 1;
    }

    isPacketOpen_ = false;

    // add end byte
    ++totalLength_;

    if (data != frameEnd_) {
        lastError_ = INVALID_LENGTH;
        return 0;
    }

    // ignore start, length, crc and end bytes
    payloadLength_ = totalLength_ - 4;

    WireCrc crc8;
    crc8.calc(&expectedLength_, 1);     // add length to CRC
    uint8_t crc = crc8.update(buffer_, payloadLength_);

    if (crc != buffer_[index_-1]) {
        lastError_ = INVALID_CRC;
        return 0;
    }

    index_ = 0;
    return 1;
}

size_t WireUnpacker::write(const uint8_t *data, size_t quantity)
{
    for (size_t i = 0; i < quantity; ++i) {
        if (!write(data[i])) {
            return i;
        }
    }
    return quantity;
}

size_t WireUnpacker::available(void)
{
    if (isPacketOpen_) return 0;

    return payloadLength_ - index_;
}

int WireUnpacker::read(void)
{
    int value = -1;
    if (!isPacketOpen_ && index_ < payloadLength_) {
        value = buffer_[index_];
        ++index_;
    }
    return value;
}

#ifdef UNPACKER_DEBUG

void WireUnpacker::printToSerial()
{
    Serial.printf("totalLen: %d, expectedLen: %d, ", totalLength_, expectedLength_);

    if (isPacketOpen_) {
        Serial.print("open, ");
    }
    else {
        Serial.print("closed, ");
    }

    switch (lastError_) {
    case INVALID_CRC:
        Serial.print("invalid crc, ");
        break;
    case INVALID_LENGTH:
        Serial.print("invalid length, ");
        break;
    default: ;
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

#endif      // ifdef UNPACKER_DEBUG

void WireUnpacker::reset()
{
    index_ = 0;
    totalLength_ = 0;
    expectedLength_ = 0;
    isPacketOpen_ = false;
    lastError_ = WireUnpacker::NONE;
}
