#include <stdio.h>

#include "utils.h"

int check_sdl_code(int code) {
	if (code < 0) {
		printf("[SDL ERROR]: %s\n", SDL_GetError());
		exit(1);
	}

	return code;
}

void* check_sdl_ptr(void* ptr) {
	if (ptr == NULL) {
		printf("[SDL ERROR]: %s\n", SDL_GetError());
		exit(1);
	}

	return ptr;
}