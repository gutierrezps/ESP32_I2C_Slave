/**
 * @file WireUnpacker.h
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief Class to unpack the data packed with WirePacker
 * @date 2020-06-16
 * 
 * WireUnpacker is used to unpack the data that was packed
 * with WirePacker and sent to this device, be it
 * master->slave or slave->master.
 * 
 * After creating the unpacker object, collect packet bytes
 * with write(). After a complete and valid packet was read,
 * the payload (data) can be read by using available() and
 * read() methods.
 * 
 * lastError() will indicate if there was an error while
 * collecting packet bytes, such as invalid length,
 * premature ending or invalid crc.
 * 
 * Expected packet format:
 *      [0]: start byte (0x02)
 *      [1]: packet length
 *      [2]: data[0]
 *      [3]: data[1]
 *      ...
 *      [n+1]: data[n-1]
 *      [n+2]: CRC8 of packet length and data
 *      [n+3]: end byte (0x04)
 * 
 */
#ifndef WireUnpacker_h
#define WireUnpacker_h

#include <Arduino.h>

#define UNPACKER_BUFFER_LENGTH 128

// #define UNPACKER_DEBUG

class WireUnpacker
{
public:

    enum Error : char
    {
        NONE = 0,
        INVALID_CRC,
        INVALID_LENGTH
    };

    WireUnpacker();
    
    /**
     * Collect a packet byte. Returns 0 if the byte was ignored
     * or if there was an error (check with lastError()).
     * 
     * The byte will be ignored if a start byte wasn't collected,
     * or if a end byte was read. In the last case, if there wasn't
     * errors, use available() and read() to read the payload.
     * 
     * @param data      byte to be collected
     * @return size_t   1 if the byte was collected
     */
    size_t write(uint8_t data);

    /**
     * Collect multiple bytes. Calls write() for every byte.
     * 
     * @param data      bytes to be collected
     * @param quantity  number of bytes to collect
     * @return size_t   number of bytes collected
     */
    size_t write(const uint8_t *data, size_t quantity);

    /**
     * Returns number of payload bytes available to be read.
     * Will also return 0 if the packet wasn't processed.
     * 
     * @return size_t
     */
    size_t available();

    /**
     * Read the next available payload byte. At each call,
     * the value returned by available() will be decremented.
     * 
     * @return int  -1 if there are no bytes to be read
     */
    int read();

    /**
     * Resets the unpacking process.
     */
    void reset();

    bool hasError() const
    {
        return lastError_ != Error::NONE;
    }
    
    Error lastError() const
    {
        return lastError_;
    }

    /**
     * Returns true if a start byte was read and more packet
     * bytes are expected.
     * 
     */
    bool isPacketOpen() const
    {
        return isPacketOpen_;
    }

    /**
     * Returns number of packet bytes read so far.
     * 
     */
    uint16_t totalLength() const
    {
        return totalLength_;
    }

    /**
     * Debug. Prints packet data to Serial.
     * 
     */
    #ifdef UNPACKER_DEBUG
    void printToSerial();
    #endif

private:
    const uint8_t frameStart_ = 0x02;
    const uint8_t frameEnd_ = 0x04;

    uint8_t buffer_[UNPACKER_BUFFER_LENGTH];
    uint8_t index_;
    uint8_t totalLength_;
    uint8_t payloadLength_;
    bool isPacketOpen_;
    uint8_t expectedLength_;
    uint8_t expectedCrc_;

    Error lastError_;
};

#endif
