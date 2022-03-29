#include "common.hh"

inline uint64_t byteswap64(uint64_t value) {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&             \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (value >> 0x38 & 0x00000000000000FF) |
         (value >> 0x28 & 0x000000000000FF00) |
         (value >> 0x18 & 0x0000000000FF0000) |
         (value >> 0x08 & 0x00000000FF000000) |
         (value << 0x08 & 0x000000FF00000000) |
         (value << 0x10 & 0x0000FF0000000000) |
         (value << 0x28 & 0x00FF000000000000) |
         (value << 0x38 & 0xFF00000000000000);
#else
  return value;
#endif
}

inline uint32_t byteswap32(uint32_t value) {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&             \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (value >> 0x18 & 0x000000FF) | (value >> 0x08 & 0x0000FF00) |
         (value << 0x08 & 0x00FF0000) | (value << 0x18 & 0xFF000000);
#else
  return value;
#endif
}

inline uint16_t byteswap16(uint16_t value) {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&             \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  return (value >> 0x08 & 0x00FF) | (value << 0x08 & 0xFF00);
#else
  return value;
#endif
}
