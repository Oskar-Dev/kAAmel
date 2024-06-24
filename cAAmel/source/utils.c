#include <stdio.h>

#include "utils.h"

cJSON* cJSON_from_file(char* file_path) {
	char* buffer = NULL;
	FILE* f;

	if (fopen_s(&f, file_path, "r") != 0) {
		goto error;
	}

	if (fseek(f, 0, SEEK_END) < 0) {
		goto error;
	}

	long m = ftell(f);
	if (m < 0) {
		goto error;
	}

	buffer = malloc(sizeof(char) * m);
	if (buffer == NULL) {
		goto error;
	}

	if (fseek(f, 0, SEEK_SET) < 0) {
		goto error;
	}

	size_t n = fread(buffer, 1, m, f);

	if (ferror(f)) {
		goto error;
	}

	fclose(f);

	cJSON* json = cJSON_Parse(buffer);
	if (json == NULL) {
		cJSON_Delete(json);
		goto error;
	}

	free(buffer);

	return json;
error:
	if (f) {
		fclose(f);
	}

	if (buffer) {
		free(buffer);
	}

	printf("[FILE ERROR] Couldn't open the file: %s\n", file_path);

	return NULL;
}

int check_sdl_code(int code) {
	if (code < 0) {
		printf("[SDL ERROR]: %d\n", SDL_GetError());
		exit(1);
	}

	return code;
}

void* check_sdl_ptr(void* ptr) {
	if (ptr == NULL) {
		printf("[SDL ERROR]: %d\n", SDL_GetError());
		exit(1);
	}

	return ptr;
}

int maxi(int a, int b) {
	if (a > b) return a;
	return b;
}