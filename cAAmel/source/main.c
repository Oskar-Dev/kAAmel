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
#include "goal.h"

#define ADVANCEMENTS 75
#define CRITERIA 187
#define MULTI_PART_ADV 8

#define MAX_LEN 200

char adv_path[MAX_LEN];
char stats_path[MAX_LEN];

char* saves_path = "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.21/.minecraft/saves";

int update = 1;

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
	snprintf(adv_path, sizeof(adv_path), "%s/%s%s\n", saves_path, filepath, "/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json"); // Fix the UUid issue, lol.
	adv_path[strcspn(adv_path, "\n")] = 0;

	snprintf(stats_path, sizeof(adv_path), "%s/%s%s\n", saves_path, filepath, "/stats/ac4cd426-a465-48f2-9217-4ed05336f4a2.json"); // Fix the UUid issue, lol.
	stats_path[strcspn(stats_path, "\n")] = 0;

	update = 1;
}

int main() {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));
	check_sdl_code(IMG_Init(IMG_INIT_PNG));

	Tracker tracker = { 0 };
	tracker_create(VERSION_1_21, &tracker);

	SDL_Window* o_window = check_sdl_ptr(SDL_CreateWindow("cAAmel - Stream Overlay", 0, 30, tracker.o_window_width, tracker.o_window_height, 0));
	SDL_Renderer* o_renderer = check_sdl_ptr(SDL_CreateRenderer(o_window, -1, SDL_RENDERER_ACCELERATED));
	// SDL_RenderSetVSync(o_renderer, 1);

	SDL_Window* m_window = check_sdl_ptr(SDL_CreateWindow("cAAmel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, tracker.m_window_width, tracker.m_window_height, 0));
	SDL_Renderer* m_renderer = check_sdl_ptr(SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED));
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

	int goals_n = 6;
	Goal** goals = goal_init(goals_n);
	goals[0] = goal_create(o_renderer, GOALTYPE_nautilus_shells);
	goals[1] = goal_create(o_renderer, GOALTYPE_trident);
	goals[2] = goal_create(o_renderer, GOALTYPE_wither_skulls);
	goals[3] = goal_create(o_renderer, GOALTYPE_sniffers);
	goals[4] = goal_create(o_renderer, GOALTYPE_heavy_core);
	goals[5] = goal_create(o_renderer, GOALTYPE_silence);

	ADV_advancement** advancements = ADV_get_advancements(tracker.advancements, tracker.template_path);

	// Dmon.
	dmon_init();
	dmon_watch(saves_path, watch_callback, 0, NULL);

	// IMAGES. //
	SDL_Texture* adv_bg = check_sdl_ptr(IMG_LoadTexture(o_renderer, "resources/gui/advancement_background.png"));
	SDL_Texture* adv_bg_done = check_sdl_ptr(IMG_LoadTexture(o_renderer, "resources/gui/advancement_background_done.png"));
	SDL_Texture* adv_bg_half_done = check_sdl_ptr(IMG_LoadTexture(o_renderer, "resources/gui/advancement_background_half_done.png"));

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
	Uint32 ticks_start = 0;
	int overlay_render_criteria = 1;
	int overlay_render_advancements = 1;
	int quit = 0;
	// SDL_SetHintWithPriority(SDL_HINT_THREAD_PRIORITY_POLICY, 1, SDL_HINT_OVERRIDE);

	// MAIN LOOP. //
	while (!quit) {
		ticks_start = SDL_GetTicks();
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
				quit = 1;
			}
		}

		// Update advancements.
		if (update) {
			ADV_update_advancements(advancements, tracker.advancements, goals, goals_n, adv_path);
			goal_update(goals, goals_n, stats_path);
		}

		tracker_update_overlay(advancements, tracker.advancements, tracker.criteria, tracker.multi_part_advancements, tracker.overlay_layout);

		// Render advancements.
		if (update) {
			tracker_render_main(m_renderer, main_font, advancements, tracker.advancements, tracker.m_window_width, tracker.m_window_height, tracker.main_layout);
			update = 0;
		}
		tracker_render_overlay(
			o_renderer, 
			overlay_font, 
			adv_bg,
			adv_bg_half_done,
			adv_bg_done,
			advancements, 
			tracker.advancements, 
			tracker.criteria, 
			tracker.multi_part_advancements, 
			goals,
			goals_n,
			tracker.o_window_width, 
			tracker.o_window_height, 
			tracker.overlay_layout
		);
		
		if (SDL_GetTicks() - ticks_start <= 20) SDL_Delay(20 - (SDL_GetTicks() - ticks_start));
	}

	for (int i = 0; ADVANCEMENTS < 1; ++i) {
		ADV_delete_advancement(advancements[i]);
	}

	dmon_deinit();
	FC_FreeFont(main_font);
	FC_FreeFont(overlay_font);
	SDL_DestroyTexture(adv_bg);
	SDL_DestroyTexture(adv_bg_done);
	SDL_DestroyTexture(adv_bg_half_done);
	free(advancements);
	tracker_delete(&tracker);
	goal_delete(goals, goals_n);
	IMG_Quit();
	SDL_Quit();

	return 0;
}