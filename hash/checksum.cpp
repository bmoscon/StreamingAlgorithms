#include <string>
#include <stdint.h>

uint8_t checksum_8(const std::string &s) {
  uint8_t checksum = 0;

  for (uint32_t i = 0; i < s.size(); ++i) {
    checksum = (checksum >> 1) + ((checksum & 1) << 7);
    checksum += s[i];
    checksum &= 0xff;
  }

  return (checksum);

}

uint16_t checksum_16(const std::string &s) {
  uint16_t checksum = 0;

  for (uint32_t i = 0; i < s.size(); ++i) {
    checksum = (checksum >> 1) + ((checksum & 1) << 15);
    checksum += s[i];
    checksum &= 0xffff;
  }

  return (checksum);
}


uint32_t checksum_32(const std::string &s) {
  uint32_t checksum = 0;

  for (uint32_t i = 0; i < s.size(); ++i) {
    checksum = (checksum >> 1) + ((checksum & 1) << 31);
    checksum += s[i];
    checksum &= 0xffffffff;
  }

uint64_t checksum_64(const std::string &s) {
  uint64_t checksum = 0;

  for (uint32_t i = 0; i < s.size(); ++i) {
    checksum = (checksum >> 1) + ((checksum & 1) << 63);
    checksum += s[i];
    checksum &= 0xffffffffffffffff;
  }

  return (checksum);
}
