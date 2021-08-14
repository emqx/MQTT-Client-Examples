#ifndef __PRC_EXCEPT_H
#define __PRC_EXCEPT_H

#include <string>

class prc_except {
public:
    enum except_type {
        None,
        FileOpenFailed,
        ArgumentError,
        OtherError,
    };

    prc_except(except_type type, std::string message)
        : m_type(type), m_message("error: " + message) {}
    except_type type() const { return m_type; }
    const std::string& message() const { return m_message; }

private:
    except_type m_type;
    std::string m_message;
};

class prc_openfile_error : public prc_except {
public:
    prc_openfile_error(std::string path)
        : prc_except(FileOpenFailed, "the file or directory `" + path + "' open failed.") {}
};

#endif
