/**
 * @file WireCrc.h
 * @author Gutierrez PS <https://github.com/gutierrezps>
 * @brief CRC8-MAXIM algorithm used in WirePacker
 * @date 2020-06-16
 * 
 * Based on <https://www.devcoons.com/crc8/>
 * and FastCRC <https://github.com/FrankBoesing/FastCRC>
 * 
 * Use calc() for the first call, and update() to 
 * feed the CRC calculation with more data.
 */

#ifndef WireCrc_h
#define WireCrc_h

#include <stdint.h>

class WireCrc
{
public:
    WireCrc() {}

    /**
     * Starts a new CRC8 calculation.
     * 
     * @param data      byte array
     * @param length    number of bytes
     * @return uint8_t  crc
     */
    uint8_t calc(uint8_t *data, unsigned int length) {
        seed = 0;
        return update(data, length);
    }

    /**
     * Feed more data in the CRC calculation. calc() must
     * be called the first time, before using update()
     * multiple times.
     * 
     * @param data      byte array
     * @param length    number of bytes
     * @return uint8_t  crc
     */
    uint8_t update(uint8_t *data, unsigned int length) {
        uint8_t crc = seed;
        uint8_t extract;
        uint8_t sum;

        for (unsigned int i = 0; i < length; i++) {
            extract = *data;

            for (char j = 8; j; j--) {
                sum = (crc ^ extract) & 0x01;
                crc >>= 1;
                if (sum) {
                    crc ^= 0x8C;
                }
                extract >>= 1;
            }

            data++;
        }

        return crc;
    }

private:
    uint8_t seed = 0;
};

#endif
