#ifndef __PRC_STRUCT_H
#define __PRC_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

struct prcheader {
    uint8_t info[4]; // magic and version
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t filesize;
    uint32_t filecount;
    uint32_t data_pos;
};

struct prcnode {
    uint16_t hash;
    uint16_t next;
    uint32_t str_pos;
    uint32_t data_pos;
    uint32_t data_len;
};

const struct prcheader* prc_header(const struct prc *prc);

#ifdef __cplusplus
}
#endif

#endif
