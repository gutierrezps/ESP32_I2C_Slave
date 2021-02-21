# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.3.0] - 2021-02-21

### Fixed

- [pr #9]: Ensured non-blocking update() loop;
- [pr #9]: Changed onRequest calling policy: now it's called only when receiving
an empty packet.

## [0.2.0] - 2020-11-23

### Added

- ESP32 architecture guard, in order to allow usage of WirePacker and WireUnpacker
on other architectures.

### Fixed

- [issue #5]:  Unbound write to rxBuffer

## [0.1.0] - 2020-06-17

Initial version.

[Unreleased]: https://github.com/gutierrezps/ESP32_I2C_Slave/compare/v0.2.0...HEAD
[0.3.0]: https://github.com/gutierrezps/ESP32_I2C_Slave/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/gutierrezps/ESP32_I2C_Slave/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/gutierrezps/ESP32_I2C_Slave/releases/tag/v0.1.0
[issue #5]: https://github.com/gutierrezps/ESP32_I2C_Slave/issues/5
[pr #9]: https://github.com/gutierrezps/ESP32_I2C_Slave/pull/9
