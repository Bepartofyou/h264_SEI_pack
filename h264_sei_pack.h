#ifndef _H264_SEI_PACK_H_INCLUDE
#define _H264_SEI_PACK_H_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

enum sei_payload_type_e
{
    /* libx264 raw enum */
    SEI_BUFFERING_PERIOD       = 0,
    SEI_PIC_TIMING             = 1,
    SEI_PAN_SCAN_RECT          = 2,
    SEI_FILLER                 = 3,
    SEI_USER_DATA_REGISTERED   = 4,
    SEI_USER_DATA_UNREGISTERED = 5,
    SEI_RECOVERY_POINT         = 6,
    SEI_DEC_REF_PIC_MARKING    = 7,
    SEI_FRAME_PACKING          = 45,

    /* self define */
    SEI_SELF_DEFINE_LAYOUT     = 100,
};


void
h264_sei_pack_internal(uint8_t *sei, int *len, uint8_t *payload, int payload_size, int payload_type)
{
    static unsigned char start_code[] = { 0x00, 0x00, 0x00, 0x01 };

    int i, index = 0;
    
    /* start code */
    memcpy(sei + index, start_code, 4);
    index += 4;

    /* nalu type */
    sei[index++] = 6;

    /* sei payload type */
    for( i = 0; i <= payload_type-255; i += 255 ) {
        sei[index++] = 255;
    }
    sei[index++] = payload_type-i;

    /* sei payload size */
    for( i = 0; i <= payload_size-255; i += 255 ) {
        sei[index++] = 255;
    }
    sei[index++] = payload_size-i;

    /* sei content */
    for( i = 0; i < payload_size; i++ ) {
        sei[index++] = payload[i];
    }

    /* sei rbsp_trailing */
    sei[index++] = 0x80;
    *len = index;
}


int
h264_sei_pack(uint8_t *sei, int *len, char *opts, bool uu)
{
    static const uint8_t uuid[16] =
    {
        0xdc, 0x45, 0xe9, 0xbd, 0xe6, 0xd9, 0x48, 0xb7,
        0x96, 0x2c, 0xd8, 0x20, 0xd9, 0x23, 0xee, 0xef
    };

    int length;
    char payload[1024] = {0};
    enum sei_payload_type_e type = SEI_USER_DATA_UNREGISTERED;

    if (uu) {
        memcpy(payload, uuid, 16);
        sprintf(payload + 16, "x264 - core 148 - H.264/MPEG-4 AVC codec - "
             "Copyright 2003-2018 - http://www.videolan.org/x264.html - options: %s", opts);

    } else {
        type = SEI_SELF_DEFINE_LAYOUT;
        sprintf(payload, "%s", opts);
    }
    length = strlen(payload) + 1;

    h264_sei_pack_internal(sei, len, (uint8_t *)payload, length, type);

    return 0;
}

#endif /* _H264_SEI_PACK_H_INCLUDE */