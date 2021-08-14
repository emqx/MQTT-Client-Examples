#ifndef __LAUNCHER_H__
#define __LAUNCHER_H__

#ifdef __cplusplus
extern "C" {
#endif

const char *get_script_path(void);
int jerry_script_parse(const char* fn);
int jerry_script_release(const char *fn);
int launch_file(const char* fn);
int launcher(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
