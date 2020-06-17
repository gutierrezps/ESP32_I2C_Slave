/**
 * @file WireSlaveRequest.h
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief Helper class to read packed data from an ESP32 slave
 * @date 2020-06-17
 * 
 * This class is made to wrap the requestFrom() from default
 * Wire library. It requests data from an ESP32 slave (packed
 * using WirePacker) until a complete packet is received. It
 * also retries and handles errors.
 * 
 * After creating an object, call the request() method.
 * If a packet was read correctly, use methods available()
 * and read() to get the payload bytes.
 * 
 * Use setRetryDelay() and setAttemps() if errors are
 * happening frequently.
 * 
 */
#ifndef WireSlaveRequest_h
#define WireSlaveRequest_h

#include <stdint.h>
#include <Wire.h>
#include "WirePacker.h"
#include "WireUnpacker.h"

class WireSlaveRequest
{
public:
    enum Status
    {
        NONE,
        PACKET_READ,
        SLAVE_NOT_FOUND,
        PACKET_ERROR,
        MAX_ATTEMPTS,
    };

    /**
     * Construct a new WireSlaveRequest object
     * 
     * @param wire              TwoWire object (Wire or Wire1)
     * @param address           slave address
     * @param responseLength    max payload length
     */
    WireSlaveRequest(TwoWire &wire, uint8_t address, uint16_t responseLength);

    /**
     * Delay in milliseconds between retry attempts
     */
    void setRetryDelay(unsigned long retryDelay)
    {
        retryDelay_ = retryDelay;
    }

    /**
     * Number of read attempts before giving up with an error status
     * 
     * @param attempts 
     */
    void setAttempts(uint8_t attempts)
    {
        maxAttempts_ = attempts;
    }

    /**
     * @brief Requests data from an ESP32 I2C slave, packed with WirePacker.
     * 
     * If a packet was successfully retrieved, methods available() and
     * read() can be used to retrieve the payload. Otherwise, check
     * lastStatus() or lastStatusString() to see what went wrong.
     * 
     * @param address   slave address (optional)
     * @return true     a new packet was read
     * @return false    something wrong happened
     */
    bool request(uint8_t address = 0);

    Status lastStatus() const
    {
        return lastStatus_;
    }

    String lastStatusToString() const;

    /**
     * Returns how many payload bytes are available to be read, after
     * a packet was successfully read through request().
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

private:
    TwoWire &wire_;
    uint8_t address_;
    uint8_t readLength_;
    unsigned long retryDelay_;
    uint8_t maxAttempts_;
    Status lastStatus_;

    uint8_t rxBuffer_[UNPACKER_BUFFER_LENGTH];
    uint16_t rxLength_;
    uint16_t rxIndex_;

    /**
     * @brief Sends an empty packet to the slave in order to trigger
     * its output buffer update.
     * 
     */
    void triggerUpdate();
};

#endif
