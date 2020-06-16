// WireSlave Sender
// by Gutierrez PS
// based on the example of the same name by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// This example code is in the public domain.


#include <Arduino.h>
#include <WireSlave.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void setup()
{
    Serial.begin(115200);

    bool res = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
    if (!res) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    WireSlave.onRequest(requestEvent);
}

void loop()
{
    WireSlave.update();

    delay(1);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
byte y = 0;

void requestEvent()
{
    // respond with message of 6 bytes
    // as expected by master
    WireSlave.print("y is ");
    WireSlave.write(y++);
}
