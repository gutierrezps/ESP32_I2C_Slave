/**
 * @file WirePacker.h
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief Class to pack the data before sending to other I2C device
 * @date 2020-06-16
 * 
 * WirePacker is used to pack the data before sending it to
 * another I2C device, be it master->slave or slave->master.
 * 
 * After creating the packer object, add data with write()
 * or with Print methods such as printf(). When finished,
 * call end() to close the packet.
 * 
 * After that, use available() and read() methods to
 * read each packet byte and send to the other device.
 * 
 * Packet format:
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
#ifndef WirePacker_h
#define WirePacker_h

#include <Arduino.h>
#include <Print.h>

#define PACKER_BUFFER_LENGTH 128

// #define PACKER_DEBUG

class WirePacker : public Print
{
public:
    WirePacker();
    
    /**
     * Add a byte to the packet, only if end() was not called yet.
     * 
     * @param data      byte to be added
     * @return size_t   1 if the byte was added
     */
    size_t write(uint8_t data);

    /**
     * Add a number of bytes to the packet. The number of bytes added
     * may be different from quantity if the buffer becomes full.
     * 
     * @param data      byte array to be added
     * @param quantity  number of bytes to add
     * @return size_t   number of bytes added
     */
    size_t write(const uint8_t *data, size_t quantity);

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

    /**
     * Returns packet length so far
     * 
     * @return size_t 
     */
    size_t packetLength() const
    {
        if (isPacketOpen_) {
            return totalLength_ + 2;
        }
        return totalLength_;
    }

    /**
     * Closes the packet. After that, use avaiable() and read()
     * to get the packet bytes.
     * 
     */
    void end();

    /**
     * Returns how many packet bytes are available to be read.
     * 
     * @return size_t
     */
    size_t available();

    /**
     * Read the next available packet byte. At each call,
     * the value returned by available() will be decremented.
     * 
     * @return int  -1 if there are no bytes to be read
     */
    int read();

    /**
     * Resets the packing process.
     * 
     */
    void reset();

    /**
     * Debug. Prints packet data to Serial.
     * 
     */
    #ifdef PACKER_DEBUG
    void printToSerial();
    #endif

private:
    const uint8_t frameStart_ = 0x02;
    const uint8_t frameEnd_ = 0x04;

    uint8_t buffer_[PACKER_BUFFER_LENGTH];
    uint8_t index_;
    uint8_t totalLength_;
    bool isPacketOpen_;
};

#endif
