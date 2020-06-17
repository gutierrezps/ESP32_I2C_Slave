// WireSlave Sender
// by Gutierrez PS <https://github.com/gutierrezps>
// ESP32 I2C slave library: <https://github.com/gutierrezps/ESP32_I2C_Slave>
// based on the example by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the WireSlave library for ESP32.
// Sends data as an I2C/TWI slave device; data is packed using WirePacker.
// In order to the slave send the data, an empty packet must
// be received first. This is internally done by the WireSlaveRequest class.
// The data is sent using WirePacker, also done internally by WireSlave.
// Refer to the "master_reader" example for use with this

#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_SLAVE_ADDR 0x04

void requestEvent();

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
    // the slave response time is directly related to how often
    // this update() method is called, so avoid using long delays
    // inside loop()
    WireSlave.update();

    // let I2C and other ESP32 peripherals interrupts work
    delay(1);
}

// function that executes whenever **a data packet is sent by master**,
// since there's no way to know (at the moment) when master
// will request or is requesting data.
// this function is registered as an event, see setup().
// do not perform time-consuming tasks inside this function,
// do them elsewhere and simply read the data you wish to
// send inside here.
void requestEvent()
{
    static byte y = 0;
    
    WireSlave.print("y is ");
    WireSlave.write(y++);
}
