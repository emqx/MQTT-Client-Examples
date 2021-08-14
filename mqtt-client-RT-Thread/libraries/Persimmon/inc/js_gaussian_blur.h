#pragma once

extern "C" {
#include <rtgui/blit.h>
}

struct rtgui_dc *gaussian_blur_3(struct rtgui_dc *dc_src);
