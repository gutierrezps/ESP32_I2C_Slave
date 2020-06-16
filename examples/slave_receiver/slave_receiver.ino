// WireSlave Receiver
// by Gutierrez PS
// based on the example by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// This example code is in the public domain.


#include <Arduino.h>
#include <WireSlave.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void receiveEvent(int howMany);

void setup()
{
    Serial.begin(115200);

    bool success = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    WireSlave.onReceive(receiveEvent);
}

void loop()
{
    WireSlave.update();

    delay(1);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
    while (1 < WireSlave.available()) // loop through all but the last
    {
        char c = WireSlave.read(); // receive byte as a character
        Serial.print(c);         // print the character
    }

    int x = WireSlave.read();    // receive byte as an integer
    Serial.println(x);         // print the integer
}