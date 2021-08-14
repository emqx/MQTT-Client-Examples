#ifndef __PRC_DIFF_H
#define __PRC_DIFF_H

#include <string>
#include <vector>

class prc_diff {
public:
    enum filetype {
        Unmodified,
        Added,
        Removed,
        Modified
    };
    struct diffnode {
        filetype type;
        std::string path;
    };
    typedef std::vector<diffnode> diff_vector;

    prc_diff(struct prc *base, struct prc *current);
    prc_diff(const std::string &base, const std::string &current);
    const diff_vector& difference() const {
        return m_difference;
    }
    void dump() const;

private:
    void compare(struct prc *base, struct prc *current);
    void comapre_file(struct prc *base, struct prc *current, struct prcnode *node);

private:
    diff_vector m_difference;
};

class prc_diff_pack {
public:
    prc_diff_pack(const std::string &base, const std::string &current);
    ~prc_diff_pack();

    void package(const std::string &path);

private:
    void scan();
    void make_flags(FILE *fp, const std::vector<bool> &flags);
    void make_records(FILE *fp);
    void write_file(FILE *fp, const std::string &path);

private:
    std::string m_curpath;
    prc_diff *m_diff;
    std::vector<bool> m_modify_flags;
    struct prc *m_base_prc, *m_current_prc;
};

#endif
