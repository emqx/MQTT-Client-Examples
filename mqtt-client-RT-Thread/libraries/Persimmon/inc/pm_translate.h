/*
 * File      : pm_translate.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef PM_TRANSLATE_H
#define PM_TRANSLATE_H

#include <string>
#include <map>

namespace Persimmon
{

class Translate
{
public:
    Translate();
    bool loadTranslateFile(const std::string fileName);
    void insert(const std::string &src, const std::string &trans);
    std::string translate(const std::string &str);

private:
    std::string readString(int fd, char *buf, size_t size, int split, bool *res);

private:
    std::map<std::string, std::string> m_map;
};

};

#endif
