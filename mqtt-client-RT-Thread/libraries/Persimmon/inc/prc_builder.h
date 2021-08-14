#ifndef __PRC_BUILDER
#define __PRC_BUILDER

#include <string>

class hash_map;

class prc_builder {
public:
    prc_builder(const std::string &file, const std::string &basepath);
    ~prc_builder();
    void set_align(int align);
    void build(hash_map &map);

private:
    struct readbuf {
        void *ptr;
        size_t size;
    };
    const void* readfile(const std::string &filename, size_t filesize);
    uint32_t calc_align(uint32_t position) const;
    void fill_align_data();

private:
    uint32_t m_align;
    FILE *m_file;
    readbuf m_readbuf;
    std::string m_outfile, m_basepath;
};

#endif // !__PRC_BUILDER
