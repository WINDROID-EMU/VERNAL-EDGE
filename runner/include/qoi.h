#ifndef QOI_H
#define QOI_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GM_QOI_INDEX    0x00
#define GM_QOI_RUN_8    0x40
#define GM_QOI_RUN_16   0x60
#define GM_QOI_DIFF_8   0x80
#define GM_QOI_DIFF_16  0xc0
#define GM_QOI_DIFF_24  0xe0
#define GM_QOI_COLOR    0xf0

#define GM_QOI_MASK_2   0xc0
#define GM_QOI_MASK_3   0xe0
#define GM_QOI_MASK_4   0xf0

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t data_len;
} gm_qoi_desc;

/**
 * Decodes GameMaker's custom QOI format to 32-bit RGBA pixel buffer.
 * Returns allocated buffer of size width * height * 4, or NULL on error.
 */
static inline uint8_t *gm_qoi_decode(const uint8_t *bytes, size_t total_size, gm_qoi_desc *desc) {
    if (!bytes || total_size < 12 || !desc) return NULL;

    /* Little-endian magic check: 'f' 'i' 'o' 'q' */
    if (bytes[0] != 'f' || bytes[1] != 'i' || bytes[2] != 'o' || bytes[3] != 'q') {
        return NULL;
    }

    uint32_t width  = (uint32_t)bytes[4] | ((uint32_t)bytes[5] << 8);
    uint32_t height = (uint32_t)bytes[6] | ((uint32_t)bytes[7] << 8);
    uint32_t length = (uint32_t)bytes[8] | ((uint32_t)bytes[9] << 8) |
                      ((uint32_t)bytes[10] << 16) | ((uint32_t)bytes[11] << 24);

    desc->width = width;
    desc->height = height;
    desc->data_len = length;

    if (width == 0 || height == 0 || 12 + length > total_size) {
        return NULL;
    }

    size_t pixel_count = (size_t)width * (size_t)height;
    uint8_t *pixels = (uint8_t *)malloc(pixel_count * 4);
    if (!pixels) return NULL;

    const uint8_t *pixel_data = bytes + 12;
    size_t pos = 0;
    int32_t run = 0;
    uint8_t r = 0, g = 0, b = 0, a = 255;
    uint8_t index[64 * 4];
    memset(index, 0, sizeof(index));

    size_t raw_len = pixel_count * 4;
    for (size_t raw_pos = 0; raw_pos < raw_len; raw_pos += 4) {
        if (run > 0) {
            run--;
        } else if (pos < length) {
            uint8_t b1 = pixel_data[pos++];

            if ((b1 & GM_QOI_MASK_2) == GM_QOI_INDEX) {
                int index_pos = (b1 ^ GM_QOI_INDEX) << 2;
                r = index[index_pos + 0];
                g = index[index_pos + 1];
                b = index[index_pos + 2];
                a = index[index_pos + 3];
            }
            else if ((b1 & GM_QOI_MASK_3) == GM_QOI_RUN_8) {
                run = b1 & 0x1f;
            }
            else if ((b1 & GM_QOI_MASK_3) == GM_QOI_RUN_16) {
                if (pos < length) {
                    uint8_t b2 = pixel_data[pos++];
                    run = (int32_t)((((uint32_t)(b1 & 0x1f)) << 8) | (uint32_t)b2) + 32;
                }
            }
            else if ((b1 & GM_QOI_MASK_2) == GM_QOI_DIFF_8) {
                int32_t b1_32 = (int32_t)b1;
                r += (uint8_t)(((b1_32 & 48) << 26) >> 30);
                g += (uint8_t)(((b1_32 & 12) << 28) >> 30);
                b += (uint8_t)(((b1_32 & 3)  << 30) >> 30);
            }
            else if ((b1 & GM_QOI_MASK_3) == GM_QOI_DIFF_16) {
                if (pos < length) {
                    uint8_t b2 = pixel_data[pos++];
                    int32_t merged = ((int32_t)b1 << 8) | (int32_t)b2;
                    r += (uint8_t)(((merged & 7936) << 19) >> 27);
                    g += (uint8_t)(((merged & 240)  << 24) >> 28);
                    b += (uint8_t)(((merged & 15)   << 28) >> 28);
                }
            }
            else if ((b1 & GM_QOI_MASK_4) == GM_QOI_DIFF_24) {
                if (pos + 1 < length) {
                    uint8_t b2 = pixel_data[pos++];
                    uint8_t b3 = pixel_data[pos++];
                    int32_t merged = ((int32_t)b1 << 16) | ((int32_t)b2 << 8) | (int32_t)b3;
                    r += (uint8_t)(((merged & 1015808) << 12) >> 27);
                    g += (uint8_t)(((merged & 31744)   << 17) >> 27);
                    b += (uint8_t)(((merged & 992)     << 22) >> 27);
                    a += (uint8_t)(((merged & 31)      << 27) >> 27);
                }
            }
            else if ((b1 & GM_QOI_MASK_4) == GM_QOI_COLOR) {
                if (b1 & 8) { if (pos < length) r = pixel_data[pos++]; }
                if (b1 & 4) { if (pos < length) g = pixel_data[pos++]; }
                if (b1 & 2) { if (pos < length) b = pixel_data[pos++]; }
                if (b1 & 1) { if (pos < length) a = pixel_data[pos++]; }
            }

            int index_pos2 = ((r ^ g ^ b ^ a) & 63) << 2;
            index[index_pos2 + 0] = r;
            index[index_pos2 + 1] = g;
            index[index_pos2 + 2] = b;
            index[index_pos2 + 3] = a;
        }

        /* Store as standard RGBA */
        pixels[raw_pos + 0] = r;
        pixels[raw_pos + 1] = g;
        pixels[raw_pos + 2] = b;
        pixels[raw_pos + 3] = a;
    }

    return pixels;
}

#ifdef __cplusplus
}
#endif

#endif /* QOI_H */
