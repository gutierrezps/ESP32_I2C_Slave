// Wire Master Reader
// by Gutierrez PS
// based on the example of the same name by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// This example code is in the public domain.

#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void setup()
{
    Serial.begin(115200);           // start serial for output
    Wire.begin(SDA_PIN, SCL_PIN);   // join i2c bus
}

void loop()
{
    // must write something before requesting data of an ESP32 slave device
    // in order to trigger the output buffer update
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(0);
    Wire.endTransmission();

    // slave response time, depends on what else the slave is doing
    // inside its loop() function
    delay(20);

    Wire.requestFrom(I2C_SLAVE_ADDR, 6);    // request 6 bytes from slave device

    while (Wire.available()) { // slave may send less than requested
        char c = Wire.read(); // receive a byte as character
        Serial.print(c);         // print the character
    }

    delay(1000);
}
