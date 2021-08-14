#pragma once

#include <string>

class FilePath
{
public:
    FilePath() {}
    FilePath(const char* path);
    FilePath(std::string path);
    ~FilePath();

    std::string dirname();
    std::string basename();

    std::string absolutePath();
    std::string relativePath(const char* base);
    std::string relativePath(std::string base);

    std::string getPath() { return _path; }
    void setPath(std::string path);

private:
    std::string simplifyPath(std::string path);

private:
    std::string _path;
};
