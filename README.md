# I2C Slave support [has been added][pr-5746] to ESP32 Arduino Framework

This library provided ESP32 Slave functionality as a workaround for the lack
of support on Arduino core and on ESP-IDF, as pointed by
[issue #118][issue-118] in 2017. On October 14, 2021, that issue was finally
closed by the [pull request #5746][pr-5746], and so this library is not
needed anymore.

See [README_old.md](README_old.md) for details about how the workaround
was implemented.

[issue-118]: https://github.com/espressif/arduino-esp32/issues/118
[pr-5746]: https://github.com/espressif/arduino-esp32/pull/5746
