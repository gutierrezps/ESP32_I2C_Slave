# ESP32 I2C Slave

This library provides ESP32 Slave functionality as a workaround for the lack
of support on Arduino core and on ESP-IDF. **There are caveats though,**
**keep reading**.

## Usage

On the slave side, simply use `WireSlave` instead of `Wire`. See examples
[slave_receiver.ino](10) and [slave_sender.ino](11).

On the master side though, data must be packed with `WirePacker` before being
sent with `Wire` (see example [master_writer.ino](12)). Reading data is a little
more tricky, so class `WireSlaveRequest` must be used (see example
[master_reader.ino](13)).

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

Only slave functionality is provided. Two global objects are available,
`WireSlave` and `WireSlave1`. If you want master and slave I2C at the same time,
use `Wire` and `WireSlave1` or `WireSlave` and `Wire1`, but not
`Wire`/`WireSlave` or `Wire1`/`WireSlave1`.

`WireSlave` setup is almost the same as `Wire`. Use `begin()` to set SDA, SCL
and address. A boolean is returned, if it's false you're probably trying to use
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
    bool success = WireSlave.begin(SDA_PIN, SCL_PIN, I2C_SLAVE_ADDR);
    if (!success) {
        Serial.println("I2C slave init failed");
        while(1) delay(100);
    }

    WireSlave.onReceive(receiveEvent);
    WireSlave.onRequest(requestEvent);

    // ...
}
```

The Slave API provided by ESP-IDF consists of two functions: one to read the
input buffer and other to write to the output buffer. Without interruption
support, the input buffer must be read periodically to know if the master has
sent something, and the output buffer must be filled before the master requests
data.

This usage of the API functions raise some problems. The first one is that when
reading the input buffer, the master may not have finished to send the data.
That means another buffer read is required to fetch the remaining bytes.
Likewise, the master may request data while the slave is filling the output
buffer. This last scenario is even more problematic since the slave may
be interrupted in the middle of a byte write, thus not only spliting but
literally breaking the data. This doesn't seem to happen in the first case, that
is, the input buffer seems to contain whole bytes only.

To mitigate this, a packing format was implemented with `WirePacker` and
`WireUnpacker`. The packet format is the following, where _n_ is the number
of data bytes, and CRC is calculated from bytes 1 to n+2 (length + data):

|            | Start | Length | Data[0] | Data[1] | ... | Data[n-1] |        CRC8       |  End |
|-----------:|:-----:|:------:|:-------:|:-------:|:---:|:---------:|:-----------------:|:----:|
| byte index |   0   |    1   |    2    |    3    | ... |    n+2    |        n+3        |  n+4 |
|      value |  0x02 |   n+4  |    x    |    x    | ... |     x     | crc of [1..(n+2)] | 0x04 |

On the slave side, the class `WireSlave` internally packs and unpacks the data
using `WirePacker` and `WireUnpacker`, so `onReceive()` and `onRequest()`
usages are **almost** the same. The only difference is that `onRequest()`
is called whenever the master sends data, since there's no way to know
when the master will request or is requesting data. Also note that
**data that is not packet will be ignored by the slave**.

On the master side, `WirePacker` is available to pack the data before sending
through `Wire`. Reading data is a little more tricky because of the data break
problem, so class `WireSlaveRequest` was created to read the packed data
from the slave, unpack it, and in case of problems it'll try to read again.

After setup, `update()` must be called periodically inside `loop()`. It's
the `update()` function that will read the input buffer and trigger
the other actions. Therefore, the slave response time is directly related
to the other things being done inside `loop()`.

## Official I2C slave support

Here are some links to follow the status of official support for I2C slave on
ESP32. As soon as it's available and well documented, this library will no
longer be needed.

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

[10]: <examples/slave_receiver/slave_receiver.ino>
[11]: <examples/slave_sender/slave_sender.ino>
[12]: <examples/master_writer/master_writer.ino>
[13]: <examples/master_reader/master_reader.ino>
