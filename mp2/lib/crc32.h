#pragma once

#ifdef __cplusplus
extern "C" {
#endif
void crc32(const void *data, size_t n_bytes, uint32_t* crc);
#ifdef __cplusplus
}
#endif