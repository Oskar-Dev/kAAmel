#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

#include "adv.h"
#include "cJSON.h"

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define SCREEN_FPS 60
#define DELTA_TIME_S (1.0f / SCREEN_FPS)
#define DELTA_TIME_MS ((Uint32) floorf(DELTA_TIME_S * 1000.0f))
#define ADVANCEMENTS 114

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


int main() {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));
	SDL_Window* window = check_sdl_ptr(SDL_CreateWindow("cAAmel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0));
	SDL_Renderer* renderer = check_sdl_ptr(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));
	SDL_Surface* window_surface = check_sdl_ptr(SDL_GetWindowSurface(window));
	check_sdl_code(SDL_UpdateWindowSurface(window));

	cJSON* data = ADV_get_json("resources/templates/1.20.6/advancements.json");
	if (data == NULL) {
		printf("[ERROR] Couldn't get data from a template.\n");
		cJSON_Delete(data);
		exit(1);
	}

	ADV_advancement** advancements = ADV_object_from_template(data, ADVANCEMENTS);
	cJSON_Delete(data);
	if (advancements == NULL) {
		printf("[ERROR] Couldn't load a template into an object.\n");
		exit(1);
	}

	for (int i = 0; i < ADVANCEMENTS; ++i) {
		int n = advancements[i]->criteria_n;

		if (n > 0) {
			printf("--- %s ---\n", advancements[i]->name);
			for (int j = 0; j < n; ++j) {
				printf("%s\n", advancements[i]->criteria[j]->name);
			}
		}
	}

	// char* camel = "resources/sprites/animals/camel.png";
	// SDL_Texture* camel_texture = check_sdl_ptr(IMG_LoadTexture(renderer, camel));
	// SDL_Rect camel_rect = { 0, 0, 64, 64 };

	// int speed = 10;
	// int dx = speed;
	// int dy = speed;
	int size = 64;
	int padding = 8;
	SDL_Rect rect = { 0, 0, size, size };

	int quit = 0;
	float t = 0.0f;

	while (!quit) {
		SDL_Event event;
		
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;

			default:
				break;
			}
		}

		// camel_rect.x += dx;
		// camel_rect.y += dy;

		// if (camel_rect.y + camel_rect.h > WINDOW_HEIGHT || camel_rect.y < 0) { dy = -dy; }
		// if (camel_rect.x + camel_rect.w > WINDOW_WIDTH || camel_rect.x < 0) { dx = -dx; }

		ADV_update_advancements(advancements, ADVANCEMENTS, "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.20.6/.minecraft/saves/FILE TEST/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json");

		check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
		check_sdl_code(SDL_RenderClear(renderer));

		for (int i = 0; i < ADVANCEMENTS; ++i) {
			if (advancements[i]->done) {
				check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255));
			} else {
				check_sdl_code(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
			}

			rect.x = (i % (WINDOW_WIDTH / (size + padding))) * (size + padding) + padding;
			rect.y = (i / (WINDOW_WIDTH / (size + padding))) * (size + padding) + padding;
			SDL_RenderFillRect(renderer, &rect);
		}

		// check_sdl_code(SDL_RenderCopy(renderer, camel_texture, NULL, &camel_rect));

		SDL_RenderPresent(renderer);

		SDL_Delay(DELTA_TIME_MS);
		t += DELTA_TIME_S;
	}

	for (int i = 0; ADVANCEMENTS < 1; ++i) {
		ADV_delete_advancement(advancements[i]);
	}

	free(advancements);

	// SDL_DestroyTexture(camel_texture);
	SDL_Quit();

	return 0;
}