// Wire Master Reader
// by Gutierrez PS <https://github.com/gutierrezps>
// ESP32 I2C slave library: <https://github.com/gutierrezps/ESP32_I2C_Slave>
// based on the example by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the WireSlaveRequest class
// Reads data from an ESP32 I2C/TWI slave device that
// uses the WireSlave library.
// Refer to the "slave_sender" example for use with this

#include <Arduino.h>
#include <Wire.h>
#include <WireSlaveRequest.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

// set the max number of bytes the slave will send.
// if the slave send more bytes, they will still be read
// but the WireSlaveRequest will perform more reads
// until a whole packet is read
#define MAX_SLAVE_RESPONSE_LENGTH 32

void setup()
{
    Serial.begin(115200);           // start serial for output
    Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
}

void loop()
{
    static unsigned long lastReadMillis = 0;

    // request data from Slave every 1000 ms
    if (millis() - lastReadMillis > 1000) {
        // first create a WireSlaveRequest object
        // first argument is the Wire bus the slave is attached to (Wire or Wire1)
        WireSlaveRequest slaveReq(Wire, I2C_SLAVE_ADDR, MAX_SLAVE_RESPONSE_LENGTH);

        // optional: set delay in milliseconds between retry attempts.
        // the default value is 10 ms
        slaveReq.setRetryDelay(5);

        // attempts to read a packet from an ESP32 slave.
        // there's no need to specify how many bytes are requested,
        // since data is expected to be packed with WirePacker,
        // and thus can have any length.
        bool success = slaveReq.request();
        
        if (success) {
            while (1 < slaveReq.available()) {  // loop through all but the last byte
                char c = slaveReq.read();       // receive byte as a character
                Serial.print(c);                // print the character
            }
            
            int x = slaveReq.read();    // receive byte as an integer
            Serial.println(x);          // print the integer
        }
        else {
            // if something went wrong, print the reason
            Serial.println(slaveReq.lastStatusToString());
        }

        lastReadMillis = millis();
    }   
}
