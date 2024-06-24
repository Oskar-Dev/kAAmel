#pragma once

#ifndef UTILS_H
#define UTILS_H

#include "../include/cJSON.h"

#include <SDL.h>

cJSON* cJSON_from_file(char* file_path);
int check_sdl_code(int code);
void* check_sdl_ptr(void* ptr);

int maxi(int a, int b);

#endif