#define DMON_IMPL

#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "adv.h"
#include "cJSON.h"
#include "SDL_FontCache.h"
#include "dirent.h"
#include "dmon.h"

#define MAIN_WINDOW_WIDTH 1600
#define MAIN_WINDOW_HEIGHT 768

#define OVERLAY_WINDOW_WIDTH 1600
#define OVERLAY_WINDOW_HEIGHT 250

#define SCREEN_FPS 60
#define DELTA_TIME_S (1.0f / SCREEN_FPS)
#define DELTA_TIME_MS ((Uint32) floorf(DELTA_TIME_S * 1000.0f))

#define ADVANCEMENTS 72
#define CRITERIA 185

#define MAX_LEN 200

char final_path[MAX_LEN];
char* saves_path = "C:/Users/oski3/OneDrive/Desktop/MultiMC/instances/1.20.6/.minecraft/saves";
int update = 1;

void recentByModification(const char* path, char* recent) {
	char buffer[MAX_LEN];
	struct dirent* entry;
	time_t recenttime = 0;
	struct stat statbuf;
	DIR* dir = opendir(path);
	while (NULL != (entry = readdir(dir))) {
		sprintf(buffer, "%s/%s", path, entry->d_name);
		stat(buffer, &statbuf);
		if (statbuf.st_mtime > recenttime) {
			strncpy(recent, entry->d_name, MAX_LEN);
			recenttime = statbuf.st_mtime;
		}
	}
	closedir(dir);
}

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

void render_advancement_box(SDL_Renderer* renderer, int x, int y, int size, int width) {
	SDL_Rect rect = { x, y, size, size };

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	check_sdl_code(SDL_RenderFillRect(renderer, &rect));

	rect.x += width;
	rect.y += width;
	rect.w -= 2 * width;
	rect.h -= 2 * width;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	check_sdl_code(SDL_RenderFillRect(renderer, &rect));
}

static void watch_callback(dmon_watch_id watch_id, dmon_action action, const char* rootdir, const char* filepath, const char* oldfilepath, void* user) {
	snprintf(final_path, sizeof(final_path), "%s/%s%s\n", saves_path, filepath, "/advancements/ac4cd426-a465-48f2-9217-4ed05336f4a2.json");
	final_path[strcspn(final_path, "\n")] = 0;

	update = 1;
}

int main() {
	check_sdl_code(SDL_Init(SDL_INIT_VIDEO));
	
	SDL_Window* overlay_window = check_sdl_ptr(SDL_CreateWindow("cAAmel - Stream Overlay", 0, 30, OVERLAY_WINDOW_WIDTH, OVERLAY_WINDOW_HEIGHT, 0));
	SDL_Window* main_window = check_sdl_ptr(SDL_CreateWindow("cAAmel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, 0));
	
	SDL_Renderer* overlay_renderer = check_sdl_ptr(SDL_CreateRenderer(overlay_window, -1, SDL_RENDERER_ACCELERATED));
	SDL_Renderer* main_renderer = check_sdl_ptr(SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED));

	// Dmon.
	dmon_init();
	dmon_watch(saves_path, watch_callback, 0, NULL);

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

	// ADV_update_advancements(advancements, ADVANCEMENTS, final_path);
	ADV_criterion** criteria_ = malloc(CRITERIA * sizeof *criteria_);

	// IMAGES. //
	check_sdl_code(IMG_Init(IMG_INIT_PNG));

	// Potrzebne do ukoñczonych celów.
	SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_BLEND);

	{
		int offset = 0;
		char path_buffer[256];
		for (int i = 0; i < ADVANCEMENTS; ++i) {
			snprintf(path_buffer, sizeof(path_buffer), "resources/sprites/%s", advancements[i]->icon);
			advancements[i]->texture = check_sdl_ptr(IMG_LoadTexture(main_renderer, path_buffer));
			advancements[i]->overlay_texture = check_sdl_ptr(IMG_LoadTexture(overlay_renderer, path_buffer));

			int n = advancements[i]->criteria_n;
			if (n > 0) {
				for (int j = 0; j < n; ++j) {
					snprintf(path_buffer, sizeof(path_buffer), "resources/sprites/%s", advancements[i]->criteria[j]->icon);
					advancements[i]->criteria[j]->texture = check_sdl_ptr(IMG_LoadTexture(main_renderer, path_buffer));
					advancements[i]->criteria[j]->overlay_texture = check_sdl_ptr(IMG_LoadTexture(overlay_renderer, path_buffer));

					// Add to criteria_ array.
					criteria_[j + offset] = advancements[i]->criteria[j];
				}
				offset += n;
			}
		}
	}

	// FONTS. //
	FC_Font* overlay_font = FC_CreateFont();
	FC_LoadFont(overlay_font, overlay_renderer, "resources/fonts/Minecraft.ttf", 16, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	FC_Font* font = FC_CreateFont();
	FC_LoadFont(font, main_renderer, "resources/fonts/Minecraft.ttf", 11, (SDL_Colour) { 255, 255, 255, 255 }, TTF_STYLE_NORMAL);

	// LAYOUT VARIABLES. //
	const Uint8 done_blend_alpha = 200;

	const int size = 48;
	const int padding = 8;
	const int spacing_x = 14;
	const int spacing_y = 20;
	const int text_margin = 4;
	const int main_advancement_box_width = 2;

	const int criteria_start_y = 220;
	const int criterion_size = 16;
	const int criterion_spacing_y = 8;
	const int criterion_text_margin = 4;
	const int criterion_text_fix = 3;
	const int criterion_max_width = 126;

	const int overlay_padding = 10;
	const int overlay_criterion_size = 48;
	const int overlay_criteria_spacing = 10;
	const int overlay_advancements_start_y = overlay_padding * 2 + overlay_criterion_size;
	const int overlay_advancement_box_width = 2;
	const int overlay_advancement_size = 70 + 2*overlay_advancement_box_width;
	const int overlay_advancements_spacing = 20;

	const int overlay_max_criteria = OVERLAY_WINDOW_WIDTH / (overlay_criterion_size + overlay_criteria_spacing) + 2;
	const int overlay_max_advancements = OVERLAY_WINDOW_WIDTH / (overlay_advancement_size + overlay_advancements_spacing) + 2;

	const int overlay_text_margin = 4;

	int overlay_advancements_offset = 0;
	int overlay_advancements_index_offset = 0;
	int overlay_criteria_offset = 0;
	int overlay_criteria_index_offset = 0;

	int scroll_speed = 15;

	SDL_Rect rect = { 0, 0, size, size };
	SDL_Rect blend_rect = { 0, 0, size + 8, size + text_margin + spacing_y };
	SDL_Rect criterion_rect = { 0, 0, criterion_size, criterion_size };
	SDL_Rect criterion_blend_rect = { 0, 0, criterion_max_width, criterion_size };

	SDL_Rect overlay_advancement_rect = { 0, overlay_advancements_start_y, overlay_advancement_size, overlay_advancement_size };
	SDL_Rect overlay_criterion_rect = { 0, overlay_padding, overlay_criterion_size, overlay_criterion_size };

	// CONTROL VARIABLES. //
	int overlay_render_criteria = 1;
	int overlay_render_advancements = 1;
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
		if (update) {
			ADV_update_advancements(advancements, ADVANCEMENTS, final_path);
		}

		// Update overlay offsets.
		if (overlay_render_advancements) {
			overlay_advancements_offset += scroll_speed;
			
			if (overlay_advancements_offset >= overlay_advancement_size + overlay_advancements_spacing) {
				overlay_advancements_offset -= overlay_advancement_size + overlay_advancements_spacing;

				overlay_advancements_index_offset = (overlay_advancements_index_offset + 1) % ADVANCEMENTS;

				int i = 0;
				while (advancements[overlay_advancements_index_offset]->done == 1 || advancements[overlay_advancements_index_offset]->criteria_n > 0) {
					overlay_advancements_index_offset = (overlay_advancements_index_offset + 1) % ADVANCEMENTS;
					if (++i > ADVANCEMENTS) { 
						overlay_render_advancements = 0;
						break; 
					}
				}
			}
		}

		if (overlay_render_criteria) {
			overlay_criteria_offset += scroll_speed;
			if (overlay_criteria_offset >= overlay_criterion_size + overlay_criteria_spacing) {
				overlay_criteria_offset -= overlay_criterion_size + overlay_criteria_spacing;

				overlay_criteria_index_offset = (overlay_criteria_index_offset + 1) % CRITERIA;

				int i = 0;
				while (criteria_[overlay_criteria_index_offset]->done == 1) {
					overlay_criteria_index_offset = (overlay_criteria_index_offset + 1) % CRITERIA;
					if (++i > CRITERIA) { 
						overlay_render_criteria = 0;
						break; 
					}
				}
			}
		}

		// OVERLAY WINDOW RENDERING. //
		check_sdl_code(SDL_SetRenderDrawColor(overlay_renderer, 0, 51, 0, 255));
		check_sdl_code(SDL_RenderClear(overlay_renderer));
		check_sdl_code(SDL_SetRenderDrawColor(overlay_renderer, 255, 0, 0, 255));

		// RENDER THE OVERLAY. //
		if (overlay_render_criteria) {
			int overlay_rendered_criteria = 0;
			int attempts = 0;
			for (int i = overlay_criteria_index_offset; overlay_rendered_criteria < overlay_max_criteria; i = (i + 1) % CRITERIA) {
				if (criteria_[i]->done == 1) {
					if (++attempts > CRITERIA) { break; }

					continue;
				} else {
					attempts = 0;
				}

				SDL_Texture* texture = criteria_[i]->overlay_texture;

				overlay_criterion_rect.x = overlay_rendered_criteria * (overlay_criterion_size + overlay_criteria_spacing) - overlay_criteria_offset;
				check_sdl_code(SDL_RenderCopy(overlay_renderer, texture, NULL, &overlay_criterion_rect));

				++overlay_rendered_criteria;
			}
		}

		if (overlay_render_advancements) {
			int overlay_rendered_advancements = 0;
			int attempts = 0;
			for (int i = overlay_advancements_index_offset; overlay_rendered_advancements < overlay_max_advancements; i = (i + 1) % ADVANCEMENTS) {
				if (advancements[i]->done == 1 || advancements[i]->criteria_n > 0) {
					if (++attempts > CRITERIA) { break; }

					continue;
				}
				else {
					attempts = 0;
				}

				SDL_Texture* texture = advancements[i]->overlay_texture;
				char* advancement_name = advancements[i]->display_name;

				overlay_advancement_rect.x = overlay_rendered_advancements * (overlay_advancement_size + overlay_advancements_spacing) - overlay_advancements_offset;

				render_advancement_box(overlay_renderer, overlay_advancement_rect.x, overlay_advancements_start_y, overlay_advancement_size, overlay_advancement_box_width);
				check_sdl_code(SDL_RenderCopy(overlay_renderer, texture, NULL, &(SDL_Rect){overlay_advancement_rect.x + 5, overlay_advancement_rect.y + 5, overlay_advancement_rect.w - 10, overlay_advancement_rect.h - 10}));

				int x = overlay_advancement_rect.x + overlay_advancement_size / 2;
				int y = overlay_advancement_rect.y + overlay_advancement_size + overlay_text_margin;
				FC_DrawAlign(overlay_font, overlay_renderer, x, y, FC_ALIGN_CENTER, advancement_name);

				++overlay_rendered_advancements;
			}
		}

		SDL_RenderPresent(overlay_renderer);

		// MAIN WINDOW RENDERING. //
		if (update) {
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
						SDL_Texture* txt = advancements[i]->criteria[j]->texture;

						char* criterion_name = advancements[i]->criteria[j]->name;
						criterion_rect.x = padding + criterion_max_width * offset + criterion_max_width * (criteria_offset + j / ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding) / (criterion_size + criterion_spacing_y)));
						criterion_rect.y = criteria_start_y + (criterion_size + criterion_spacing_y) * (j % ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding) / (criterion_size + criterion_spacing_y)));

						check_sdl_code(SDL_RenderCopy(main_renderer, txt, NULL, &criterion_rect));
						FC_Draw(font, main_renderer, criterion_rect.x + criterion_size + criterion_text_margin, criterion_rect.y + criterion_text_fix, criterion_name);

						if (criterion_done) {
							criterion_blend_rect.x = criterion_rect.x;
							criterion_blend_rect.y = criterion_rect.y;

							check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, done_blend_alpha));
							check_sdl_code(SDL_RenderFillRect(main_renderer, &criterion_blend_rect));
						}
					}

					criteria_offset += criteria / ((MAIN_WINDOW_HEIGHT - criteria_start_y - padding - criterion_size) / (criterion_size + criterion_spacing_y));
					offset += 1;
					continue;
				}

				char* advancement_name = advancements[i]->display_name;

				rect.x = ((i - offset) % (MAIN_WINDOW_WIDTH / (size + spacing_x))) * (size + spacing_x) + padding;
				rect.y = ((i - offset) / (MAIN_WINDOW_WIDTH / (size + spacing_x))) * (size + spacing_y) + padding;

				render_advancement_box(main_renderer, rect.x - main_advancement_box_width, rect.y - main_advancement_box_width, size + 2 * main_advancement_box_width, main_advancement_box_width);
				check_sdl_code(SDL_RenderCopy(main_renderer, advancements[i]->texture, NULL, &(SDL_Rect){rect.x + 8, rect.y + 8, 32, 32}));

				int x = rect.x + size / 2;
				int y = rect.y + size + text_margin;
				FC_DrawAlign(font, main_renderer, x, y, FC_ALIGN_CENTER, advancement_name);

				if (done) {
					blend_rect.x = rect.x - 4;
					blend_rect.y = rect.y - 2;

					check_sdl_code(SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, done_blend_alpha));
					check_sdl_code(SDL_RenderFillRect(main_renderer, &blend_rect));
				}
			}

			SDL_RenderPresent(main_renderer);
			update = 0;
		}

		// Don't render too often.
		SDL_Delay(DELTA_TIME_MS);
		t += DELTA_TIME_S;
	}

	for (int i = 0; ADVANCEMENTS < 1; ++i) {
		ADV_delete_advancement(advancements[i]);
	}

	dmon_deinit();
	FC_FreeFont(font);
	FC_FreeFont(overlay_font);
	free(advancements);
	IMG_Quit();
	SDL_Quit();

	return 0;
}