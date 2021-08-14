#ifndef __PRC_H
#define __PRC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct prc prc_t;
typedef struct prcnode prcnode_t;

typedef struct prc_record {
    uint32_t rec_pos;  // record position at prc file
    uint32_t rec_size; // recode size
    uint32_t pos;      // current record position
} prc_rec_t;

#define prc_size(rec)   ((rec)->rec_size)
#define prc_tell(rec)   ((rec)->pos - (rec)->rec_pos)

prc_t* prc_open_package(const char *path);
void prc_close_package(prc_t *prc);
const char* prc_base_path(prc_t *prc);
void prc_set_base_path(prc_t *prc, const char *path);
uint32_t prc_strhash(const char *str);
const char* prc_trans_url(prc_t *prc, const char *url);
int prc_open(prc_t *prc, const char *url, prc_rec_t *rec);
int prc_open_ex(prc_t *prc, const char *url, uint32_t hash, prc_rec_t *rec);
int prc_read(prc_t *prc, prc_rec_t *rec, void *buf, size_t len);
int prc_seek(prc_rec_t *rec, int offset);
int prc_lseek(prc_rec_t *rec, int offset, int whence);
int prc_node_open(prc_t *prc, prcnode_t *node, prc_rec_t *rec);
prcnode_t* prc_node_first(prc_t *prc);
prcnode_t* prc_node_next(prc_t *prc, prcnode_t *node);
const char* prc_node_path(prcnode_t *node);

int prc_decompress(const char *dst, const char *base, const char *diff);

#ifdef __cplusplus
}
#endif

#endif
