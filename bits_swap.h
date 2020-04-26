#ifndef BITS_SWAP_H
#define BITS_SWAP_H

#include <cinttypes>

#define AV_BSWAP16C(x) (((x) << 8 & 0xff00)  | ((x) >> 8 & 0x00ff))
#define AV_BSWAP32C(x) (AV_BSWAP16C(x) << 16 | AV_BSWAP16C((x) >> 16))
#define AV_BSWAP64C(x) (AV_BSWAP32C(x) << 32 | AV_BSWAP32C((x) >> 32))

inline uint16_t bits_swapb16(uint16_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return AV_BSWAP16C(x);
#else
    return x;
#endif
}

inline uint32_t bits_swapb32(uint32_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return AV_BSWAP32C(x);
#else
    return x;
#endif
}

inline uint64_t bits_swapb64(uint64_t x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return AV_BSWAP64C(x);
#else
    return x;
#endif
}

#endif /* BITS_SWAP_H */