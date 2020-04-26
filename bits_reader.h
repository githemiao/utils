#ifndef BITS_READER_H
#define BITS_READER_H

#include <cinttypes>
#include <cstring>

#include "bits_swap.h"

class BitsReader {
public:
    BitsReader(uint8_t *data, size_t len);
    ~BitsReader();

    uint8_t read8();
    uint16_t read16();
    uint32_t read32();
    uint64_t read64();
    int read(uint8_t *data, size_t *len);

    size_t offset();
    void setOffset(size_t offset);

private:
    uint8_t *data_ = nullptr;
    size_t offset_ = 0;
    size_t length_ = 0;
};

inline BitsReader::BitsReader(uint8_t *data, size_t len) {
    data_ = data;
    length_ = len;
}

inline BitsReader::~BitsReader() {
}

inline uint8_t BitsReader::read8() {
    return data_[offset_++];
}

inline uint16_t BitsReader::read16() {
    uint16_t *p = (uint16_t *)(&data_[offset_]);
    offset_ += 2;
    return bits_swapb16(*p);
}

inline uint32_t BitsReader::read32() {
    uint32_t *p = (uint32_t *)(&data_[offset_]);
    offset_ += 4;
    return bits_swapb32(*p);
}

inline uint64_t BitsReader::read64() {
    uint64_t *p = (uint64_t *)(&data_[offset_]);
    offset_ += 8;
    return bits_swapb64(*p);
}

inline int BitsReader::read(uint8_t *buff, size_t *len) {
    size_t size = length_ - offset_;
    *len = size > *len ? *len : size;

    memcpy(buff, &data_[offset_], *len);
    offset_ += *len;
    return *len;
}

inline size_t BitsReader::offset() {
    return offset_;
}

inline void BitsReader::setOffset(size_t offset) {
    offset_ = offset;
}

#endif /* BITS_READER_H */