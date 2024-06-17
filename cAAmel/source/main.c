#define DMON_IMPL

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "../include/cJSON.h"
#include "../include/SDL_FontCache.h"
#include "../include/dmon.h"

#include "adv.h"
#include "utils.h"
#include "tracker.h"

#define ADVANCEMENTS 75
#define CRITERIA 187
#define MULTI_PART_ADV 8

#define MAX_LEN 200

char final_path[MAX_LEN];

char* saves_path = "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.21/.minecraft/saves";

int update = 1;

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
	snprintf(final_path, sizeof(final_path), "%s/%s%s\n", saves_path, filepath, "/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json");
	final_path[strcspn(final_path, "\n")] = 0;

	update = 1;
}

static void render() {

}

int main() {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));
	check_sdl_code(IMG_Init(IMG_INIT_PNG));

	Tracker* tracker = create_tracker(VERSION_1_21);

	SDL_Window* o_window = check_sdl_ptr(SDL_CreateWindow("cAAmel - Stream Overlay", 0, 30, tracker->o_window_width, tracker->o_window_height, 0));
	SDL_Renderer* o_renderer = check_sdl_ptr(SDL_CreateRenderer(o_window, -1, SDL_RENDERER_ACCELERATED));
	SDL_RenderSetVSync(o_renderer, 1);

	SDL_Window* m_window = check_sdl_ptr(SDL_CreateWindow("cAAmel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, tracker->m_window_width, tracker->m_window_height, 0));
	SDL_Renderer* m_renderer = check_sdl_ptr(SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED));
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	ADV_advancement** advancements = ADV_get_advancements(tracker->advancements, tracker->template_path);

	// Dmon.
	dmon_init();
	dmon_watch(saves_path, watch_callback, 0, NULL);

	// IMAGES. //
	SDL_Texture* advancement_background = check_sdl_ptr(IMG_LoadTexture(o_renderer, "resources/gui/advancement_background.png"));

	// FONTS. //
	FC_Font* overlay_font = FC_CreateFont();
	FC_LoadFont(overlay_font, o_renderer, "resources/fonts/Minecraft.ttf", 16, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	FC_Font* main_font = FC_CreateFont();
	FC_LoadFont(main_font, m_renderer, "resources/fonts/Minecraft.ttf", 11, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	{
		int offset = 0;
		char path_buffer[256];
		for (int i = 0; i < ADVANCEMENTS; ++i) {
			snprintf(path_buffer, sizeof(path_buffer), "resources/sprites/%s", advancements[i]->icon);
			advancements[i]->texture = check_sdl_ptr(IMG_LoadTexture(m_renderer, path_buffer));
			advancements[i]->overlay_texture = check_sdl_ptr(IMG_LoadTexture(o_renderer, path_buffer));

			int n = advancements[i]->criteria_n;
			if (n > 0) {
				for (int j = 0; j < n; ++j) {
					snprintf(path_buffer, sizeof(path_buffer), "resources/sprites/%s", advancements[i]->criteria[j]->icon);
					advancements[i]->criteria[j]->texture = check_sdl_ptr(IMG_LoadTexture(m_renderer, path_buffer));
					advancements[i]->criteria[j]->overlay_texture = check_sdl_ptr(IMG_LoadTexture(o_renderer, path_buffer));
				}
				offset += n;
			}
		}
	}

	// CONTROL VARIABLES. //
	int overlay_render_criteria = 1;
	int overlay_render_advancements = 1;
	int quit = 0;

	// MAIN LOOP. //
	while (!quit) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
				quit = 1;
			}
		}

		// Update advancements.
		if (update) {
			ADV_update_advancements(advancements, tracker->advancements, final_path);
		}

		tracker_update_overlay(advancements, tracker->advancements, tracker->criteria, tracker->multi_part_advancements, tracker->overlay_layout);

		if (update) {
			tracker_render_main(m_renderer, main_font, advancements, tracker->advancements, tracker->m_window_width, tracker->m_window_height, tracker->main_layout);
			update = 0;
		}
		tracker_render_overlay(o_renderer, overlay_font, advancement_background, advancements, tracker->advancements, tracker->criteria, tracker->multi_part_advancements, tracker->o_window_width, tracker->o_window_height, tracker->overlay_layout);
	}

	for (int i = 0; ADVANCEMENTS < 1; ++i) {
		ADV_delete_advancement(advancements[i]);
	}

	dmon_deinit();
	FC_FreeFont(main_font);
	FC_FreeFont(overlay_font);
	SDL_DestroyTexture(advancement_background);
	free(advancements);
	delete_tracker(tracker);
	IMG_Quit();
	SDL_Quit();

	return 0;
}