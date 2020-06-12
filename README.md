# SlaveTwoWire

This library provides ESP32 Slave functionality as a workaround for the lack
of support on Arduino core and on ESP-IDF. **There are limitations though,**
**keep reading**.

## Context

Right now, the I2C Slave functionality is not implemented in ESP32 Arduino Core
([see issue #118][1]).

The ESP IDF, on the other hand, provides only two functions for the ESP32 to
[communicate as a slave device][2], and although it says a custom ISR function
can be defined, there's no example on how to do it properly (like reading
and clearing interrupt flags).

In this context, this library was made as a workaround to temporarily provide
ESP32 the ability to work as a I2C slave device. As such, it doesn't work
like default TwoWire library available on AVR and ESP8266 Arduino cores.

## How it works

Only slave functionality is provided. Two global objects are available, `SWire`
and `SWire1`. If you want master and slave I2C at the same time, use `Wire` and
`SWire1` or `SWire` and `Wire1`, but not `Wire`/`SWire` or `Wire1`/`SWire1`.

`SWire` setup is almost the same as `Wire`. Use `begin()` to set SDA, SCL and
address. A boolean is returned, if it's false you're probably trying to use
invalid pins. Methods `onReceive()` and `onRequest()` are similar to `Wire`,
but **do not behave as expected**.

```c++
// called when data is received
void receiveEvent(int howMany);

// called to fill the output buffer
void requestEvent();

void setup() {
    // ...

    // I2C slave setup
    bool success = SWire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    SWire.onReceive(receiveEvent);
    SWire.onRequest(requestEvent);

    // ...
}
```

The Slave API provided by ESP-IDF consists of two functions: one to
read the input buffer and other to write to the output buffer. Without
interruption support, the input buffer must be read periodically to know
if the master has sent something, and the output buffer must be filled
before the master request data. That's why `onReceive()` and `onRequest()`
**do not work as in default TwoWire**.

The function passed to `onReceive()` is called whenever there's data in
the I2C input buffer. **CAVEAT**: the master device may not have finished
writing into the buffer. If this happens, `receiveEvent()` will be called
twice, one with each fragment.

Since there's no way to know when the master will read data, the function
passed to `onRequest()` will also be called when the master sends data.
That is why a master reading **must** be preceded by a write, as shown in
example `master_reader`. **CAVEAT**: the slave may not have finished to write
into the output buffer when the master requests data.

## Status of official support for I2C Slave

Here are some links to follow the status of official support for I2C slave on
ESP32. As soon as it's available, this library will no longer be needed.

* Issue #118 on arduino-esp32: [I2C Slave not implemented][1]
* Issue #2202 on esp-idf: [I2C slave driver problems (IDFGH-297)][3]
* Pull request #2096 on esp-idf: [New I2c slave driver (IDFGH-2501)][4]
* Issue #3099 on esp-idf: [I2C interrupt (IDFGH-661)][5]

## Resources

* ESP IDF API for I2C slave: [I2C Driver][2]
* I2C example for ESP IDF API: [i2c_example_main.c][6]

[1]: <https://github.com/espressif/arduino-esp32/issues/118>
[2]: <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html#communication-as-slave>
[3]: <https://github.com/espressif/esp-idf/issues/2202>
[4]: <https://github.com/espressif/esp-idf/pull/2096>
[5]: <https://github.com/espressif/esp-idf/issues/3099>
[6]: <https://github.com/espressif/esp-idf/blob/master/examples/peripherals/i2c/i2c_self_test/main/i2c_example_main.c>
