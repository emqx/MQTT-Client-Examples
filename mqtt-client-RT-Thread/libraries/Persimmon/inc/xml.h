#ifndef __XML_H__
#define __XML_H__

#include <ezxml.h>

#ifdef __cplusplus
extern "C" {
#endif

ezxml_t ezxml_structure_parse(const char* fn);
int ezxml_structure_release(const char *fn);
void ezxml_structure_empty(void);

#ifdef PKG_USING_XML_BINARY

#define ADDR_POS(d1, d2) ((pos_t)((pos_t)(d1) >= (pos_t)(d2) ? (pos_t)(d1) - (pos_t)(d2) : (pos_t)(d2) - (pos_t)(d1)))
#define POS_ADDR(addr, pos) ((void *)((pos_t)(addr) + pos))

#define START_ADDR  (0xFFFFFFFF)

#define XMLB_INIT_FLAG  (0x1 << 0)

#define XMLB_ERRL 130        // maximum error string length

#define XMLB_MAGIC  (0x424C4D58u)   //XMLB

typedef struct xmlb* xmlb_t;
typedef uint32_t pos_t;

struct xmlb
{
    pos_t name;     // tag name
    pos_t attr;     // tag attributes { name, value, name, value, ... NULL }
    pos_t txt;      // tag character content, empty string if none
    size_t off;      // tag offset from start of parent tag character content
    pos_t next;     // next tag with same name in this section at this depth
    pos_t sibling;  // next tag with different name in same section and depth
    pos_t ordered;  // next tag, same section and depth, in original order
    pos_t child;    // head of sub tag list, NULL if none
    pos_t parent;   // parent tag, NULL if current tag is root tag
    short flags;       // additional information
    short reserved;    // reserved field
};

struct xmlb_head
{
    unsigned int magic;
    pos_t root_pos;
    pos_t root_attr_pos;
    pos_t root_attr_value_pos;
    pos_t root_pi_pos;
    pos_t root_pi_value_pos;
    pos_t root_ent_pos;
    pos_t xmlb_pos;
    pos_t xmlb_attr_pos;
    pos_t xmlb_str_pos;

    pos_t root_attr_cnt;
    pos_t root_attr_value_cnt;
    pos_t root_pi_cnt;
    pos_t root_pi_value_cnt;
    pos_t root_ent_cnt;
    pos_t xmlb_cnt;
    pos_t xmlb_attr_cnt;
    pos_t xmlb_str_cnt;

    size_t size;
    char flags;
    char mode;
    short reserved;    // reserved field
};

ezxml_t xmlb_to_xml(struct xmlb_head* head, size_t size);
int is_xmlb(struct xmlb_head* head);

#endif

#ifdef __cplusplus
}
#endif

#endif
