#pragma once

extern "C" {

    /* Add pm object to javascript runtime */
    int js_persimmon_init(void);
    int js_persimmon_deinit(void);
}
