#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "adv.h"
#include "cJSON.h"
#include "SDL_FontCache.h"

#define MAIN_WINDOW_WIDTH 1600
#define MAIN_WINDOW_HEIGHT 768

#define OVERLAY_WINDOW_WIDTH 1800
#define OVERLAY_WINDOW_HEIGHT 250

#define SCREEN_FPS 60
#define DELTA_TIME_S (1.0f / SCREEN_FPS)
#define DELTA_TIME_MS ((Uint32) floorf(DELTA_TIME_S * 1000.0f))

#define ADVANCEMENTS 72
#define CRITERIA 187

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
	
	SDL_Window* overlay_window = check_sdl_ptr(SDL_CreateWindow("cAAmel - Stream Overlay", 0, 30, OVERLAY_WINDOW_WIDTH, OVERLAY_WINDOW_HEIGHT, 0));
	SDL_Window* main_window = check_sdl_ptr(SDL_CreateWindow("cAAmel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, 0));
	
	SDL_Renderer* overlay_renderer = check_sdl_ptr(SDL_CreateRenderer(overlay_window, -1, SDL_RENDERER_ACCELERATED));
	SDL_Renderer* main_renderer = check_sdl_ptr(SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED));

	// SDL_Surface* overlay_window_surface = check_sdl_ptr(SDL_GetWindowSurface(overlay_window));
	// SDL_Surface* main_window_surface = check_sdl_ptr(SDL_GetWindowSurface(main_window));
	
	// check_sdl_code(SDL_UpdateWindowSurface(main_window));

	cJSON* data = ADV_get_json("resources/templates/1.20.6/advancements_optimized.json");
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

	ADV_update_advancements(advancements, ADVANCEMENTS, "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.20.6/.minecraft/saves/FILE TEST/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json");

	// IMAGES. //
	// check_sdl_code(IMG_Init(IMG_INIT_PNG));


	// FONTS. //
	FC_Font* overlay_font = FC_CreateFont();
	FC_LoadFont(overlay_font, overlay_renderer, "resources/fonts/Minecraft.ttf", 16, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, main_renderer, "resources/fonts/Minecraft.ttf", 11, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	// LAYOUT VARIABLES. //
	const int size = 48;
	const int padding = 8;
	const int spacing_x = 14;
	const int spacing_y = 20;
	const int text_margin = 4;

	const int criteria_start_y = 220;
	const int criterion_size = 16;
	const int criterion_spacing_y = 8;
	const int criterion_text_margin = 4;
	const int criterion_text_fix = 3;
	const int criterion_max_width = 125;

	const int overlay_padding = 10;
	const int overlay_criterion_size = 64;
	const int overlay_criteria_spacing = 10;
	const int overlay_advancements_start_y = overlay_padding * 2 + overlay_criterion_size;
	const int overlay_advancement_size = 96;
	const int overlay_advancements_spacing = 10;

	const int overlay_max_criteria = OVERLAY_WINDOW_WIDTH / (overlay_criterion_size + overlay_criteria_spacing) + 2;
	const int overlay_max_advancements = OVERLAY_WINDOW_WIDTH / (overlay_advancement_size + overlay_advancements_spacing) + 2;

	const int overlay_text_margin = 4;

	int overlay_advancements_offset = 0;
	int overlay_advancements_index_offset = 0;
	int overlay_criteria_offset = 0;
	int overlay_criteria_index_offset = 0;

	int scroll_speed = 10;

	SDL_Rect rect = { 0, 0, size, size };
	SDL_Rect criterion_rect = { 0, 0, criterion_size, criterion_size };

	SDL_Rect overlay_advancement_rect = { 0, overlay_advancements_start_y, overlay_advancement_size, overlay_advancement_size };
	SDL_Rect overlay_criterion_rect = { 0, overlay_padding, overlay_criterion_size, overlay_criterion_size };

	// CONTROL VARIABLES. //
	int quit = 0;
	float t = 0.0f;

	// MAIN LOOP. //
	while (!quit) {
		SDL_Event event;
		
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
				quit = 1;
			}
		}

		// Update advancements.
		ADV_update_advancements(advancements, ADVANCEMENTS, "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.20.6/.minecraft/saves/FILE TEST/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json");

		// Update overlay offsets.
		overlay_advancements_offset += scroll_speed;
		if (overlay_advancements_offset >= overlay_advancement_size + overlay_advancements_spacing) {
			overlay_advancements_offset -= overlay_advancement_size + overlay_advancements_spacing;
			
			++overlay_advancements_index_offset;
			if (overlay_advancements_index_offset >= ADVANCEMENTS) {
				overlay_advancements_index_offset = 0;
			}
		}

		overlay_criteria_offset += scroll_speed;
		if (overlay_criteria_offset >= overlay_criterion_size + overlay_criteria_spacing) {
			overlay_criteria_offset -= overlay_criterion_size + overlay_criteria_spacing;

			++overlay_criteria_index_offset;
			if (overlay_criteria_index_offset >= CRITERIA) {
				overlay_criteria_index_offset = 0;
			}
		}

		// OVERLAY WINDOW RENDERING. //
		check_sdl_code(SDL_SetRenderDrawColor(overlay_renderer, 0, 0, 0, 255));
		check_sdl_code(SDL_RenderClear(overlay_renderer));
		check_sdl_code(SDL_SetRenderDrawColor(overlay_renderer, 255, 0, 0, 255));

		// RENDER THE OVERLAY. //
		int overlay_rendered_criteria = 0;
		int overlay_rendered_advancements = 0;
		for (int i = 0; i < ADVANCEMENTS; ++i) {
			if (advancements[i]->done) continue;

			// Kryteria.
			if (overlay_rendered_criteria >= overlay_max_criteria) {
				break;
			}

			++overlay_rendered_criteria;

			// Postêpy.
			int a_index = (overlay_advancements_index_offset + i) % ADVANCEMENTS;
			char* advancement_name = advancements[a_index]->display_name;

			if (overlay_rendered_advancements < overlay_max_advancements) {
				overlay_advancement_rect.x = overlay_rendered_advancements * (overlay_advancement_size + overlay_advancements_spacing) - overlay_advancements_offset;
				SDL_RenderFillRect(overlay_renderer, &overlay_advancement_rect);

				int x = overlay_advancement_rect.x + overlay_advancement_size / 2;
				int y = overlay_advancement_rect.y + overlay_advancement_size + overlay_text_margin;
				FC_DrawAlign(overlay_font, overlay_renderer, x, y, FC_ALIGN_CENTER, advancement_name);
				
				++overlay_rendered_advancements;
			}
		}

		SDL_RenderPresent(overlay_renderer);

		// MAIN WINDOW RENDERING. //
		check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255));
		check_sdl_code(SDL_RenderClear(main_renderer));
		
		// RENDER LAYOUT. //
		int offset = 0;
		int criteria_offset = 0;
		for (int i = 0; i < ADVANCEMENTS; ++i) {
			int done = advancements[i]->done;
			int criteria = advancements[i]->criteria_n;
			
			if (criteria > 0) { 
				for (int j = 0; j < criteria; ++j) {
					int criterion_done = advancements[i]->criteria[j]->done;

					if (criterion_done) {
						check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 0, 255, 0, 255));
					} else {
						check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255));
					}

					char* criterion_name = advancements[i]->criteria[j]->name;
					criterion_rect.x = padding + criterion_max_width * offset + criterion_max_width * (criteria_offset + j / ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding) / (criterion_size + criterion_spacing_y)));
					criterion_rect.y = criteria_start_y + (criterion_size + criterion_spacing_y) * (j % ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding) / (criterion_size + criterion_spacing_y)));

					SDL_RenderFillRect(main_renderer, &criterion_rect);
					FC_Draw(font, main_renderer, criterion_rect.x + criterion_size + criterion_text_margin, criterion_rect.y + criterion_text_fix, criterion_name);
				}

				criteria_offset += criteria / ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding - criterion_size) / (criterion_size + criterion_spacing_y));
				offset += 1;
				continue;
			}

			if (done) {
				check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 0, 255, 0, 255));
			}
			else {
				check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 255, 0, 0, 255));
			}

			char* advancement_name = advancements[i]->display_name;

			rect.x = ((i - offset) % (MAIN_WINDOW_WIDTH / (size + spacing_x))) * (size + spacing_x) + padding;
			rect.y = ((i - offset) / (MAIN_WINDOW_WIDTH / (size + spacing_x))) * (size + spacing_y) + padding;
			SDL_RenderFillRect(main_renderer, &rect);
			FC_DrawAlign(font, main_renderer, rect.x + size / 2, rect.y + size + text_margin, FC_ALIGN_CENTER, advancement_name);
		}

		SDL_RenderPresent(main_renderer);

		// Don't render too often.
		SDL_Delay(DELTA_TIME_MS);
		t += DELTA_TIME_S;
	}

	for (int i = 0; ADVANCEMENTS < 1; ++i) {
		ADV_delete_advancement(advancements[i]);
	}

	FC_FreeFont(font);
	free(advancements);
	SDL_Quit();

	return 0;
}