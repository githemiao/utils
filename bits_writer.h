#ifndef BITS_WRITER_H
#define BITS_WRITER_H

#include <cstring>

#include "bits_swap.h"

class BitsWriter {
public:
    BitsWriter(uint8_t *data, size_t size);

    int write8(uint8_t bytes);
    int write16(uint16_t bytes);
    int write32(uint32_t bytes);
    int write64(uint64_t bytes);
    int write(uint8_t *bytes, size_t *len);
    size_t offset();

private:
    uint8_t *data_ = nullptr;
    size_t length_ = 0;
    size_t offset_ = 0;
};

inline BitsWriter::BitsWriter(uint8_t *data, size_t size) {
    data_ = data;
    length_ = size;
}

inline int BitsWriter::write8(uint8_t bytes) {
    data_[offset_++] = bytes;
    return 0;
}

inline int BitsWriter::write16(uint16_t bytes) {
    uint16_t *p = (uint16_t *)&data_[offset_];
    *p = bits_swapb16(bytes);
    offset_ += 2;

    return 0;
}

inline int BitsWriter::write32(uint32_t bytes) {
    uint32_t *p = (uint32_t *)&data_[offset_];
    *p = bits_swapb32(bytes);
    offset_ += 4;

    return 0;
}

inline int BitsWriter::write64(uint64_t bytes) {
    uint64_t *p = (uint64_t *)&data_[offset_];
    *p = bits_swapb64(bytes);
    offset_ += 8;

    return 0;
}

inline int BitsWriter::write(uint8_t *bytes, size_t *len) {
    uint8_t *p = (uint8_t *)&data_[offset_];
    size_t size = length_ - offset_;

    *len = size > *len ? *len : size;
    memcpy(p, bytes, *len);
    offset_ += *len;

    return 0;
}

inline size_t BitsWriter::offset() {
    return offset_;
}

#endif /* BITS_WRITER_H */