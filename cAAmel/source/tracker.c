#include "tracker.h"
#include "utils.h"

Tracker* tracker_create(Version version, Tracker* tracker) {
	int advancements;
	int criteria;
	int multi_part_advancements;
	int m_window_width;
	int m_window_height;
	int o_window_width;
	int o_window_height;

	/*Tracker* tracker = malloc(sizeof * tracker);
	if (tracker == NULL) {
		printf("[ERROR] Couldn't create application data.\n");
		exit(1);
	}*/

	switch (version) {
	case VERSION_1_16:
		advancements = 43;
		criteria = 149;
		multi_part_advancements = 6;
		m_window_width = 1210;
		m_window_height = 768;
		o_window_width = 1600;
		o_window_height = 400;

		tracker->template_path = malloc((strlen("resources/templates/1.16.1/advancements_optimised.json") + 1) * sizeof(char));
		if (tracker->template_path == NULL) {
			printf("[MEMORY ERROR] Couldn't allocate enough space for application's template path.\n");
		}

		strcpy(tracker->template_path, "resources/templates/1.16.1/advancements_optimised.json");
		break;

	case VERSION_1_20_6:
		advancements = 69;
		criteria = 185;
		multi_part_advancements = 8;
		m_window_width = 1630;
		m_window_height = 768;
		o_window_width = 1600;
		o_window_height = 400;

		tracker->template_path = malloc((strlen("resources/templates/1.20.6/advancements_optimised.json") + 1) * sizeof(char));
		if (tracker->template_path == NULL) {
			printf("[MEMORY ERROR] Couldn't allocate enough space for application's template path.\n");
		}

		strcpy(tracker->template_path, "resources/templates/1.20.6/advancements_optimised.json");
		break;

	case VERSION_1_21:
		advancements = 75;
		criteria = 187;
		multi_part_advancements = 8;
		m_window_width = 1630;
		m_window_height = 768;
		o_window_width = 1600;
		o_window_height = 400;

		tracker->template_path = malloc((strlen("resources/templates/1.21/advancements_optimised.json") + 1) * sizeof(char));
		if (tracker->template_path == NULL) {
			printf("[MEMORY ERROR] Couldn't allocate enough space for application's template path.\n");
		}

		strcpy(tracker->template_path, "resources/templates/1.21/advancements_optimised.json");
		break;

	default:
		printf("[ERROR] Invalid Version.\n");
		exit(1);
	}

	tracker->advancements = advancements;
	tracker->criteria = criteria;
	tracker->multi_part_advancements = multi_part_advancements;
	tracker->m_window_width = m_window_width;
	tracker->m_window_height = m_window_height;
	tracker->o_window_width = o_window_width;
	tracker->o_window_height = o_window_height;

	// Create Layout.
	tracker->main_layout = malloc(sizeof * tracker->main_layout);
	if (tracker->main_layout == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for the main layout.\n");
	}

	tracker->main_layout->adv_size = 48;
	tracker->main_layout->padding = 8;
	tracker->main_layout->spacing_x = 14;
	tracker->main_layout->spacing_y = 20;
	tracker->main_layout->text_margin = 4;
	tracker->main_layout->box_width = 2;
	tracker->main_layout->crt_start_y = 220;
	tracker->main_layout->crt_size = 16;
	tracker->main_layout->crt_spacing_y = 8;
	tracker->main_layout->crt_spacing_x = 20;
	tracker->main_layout->crt_group_spacing_x = 40;
	tracker->main_layout->crt_text_margin = 4;
	tracker->main_layout->crt_text_fix = 3;

	tracker->overlay_layout = malloc(sizeof * tracker->overlay_layout);
	if (tracker->overlay_layout == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for the main layout.\n");
	}

	tracker->overlay_layout->padding = 10;
	tracker->overlay_layout->crt_size = 48;
	tracker->overlay_layout->crt_spacing = 10;
	tracker->overlay_layout->adv_start_y = 0;
	tracker->overlay_layout->adv_size = 64;
	tracker->overlay_layout->adv_bg_size = 96;
	tracker->overlay_layout->adv_spacing = 32;

	tracker->overlay_layout->text_margin = 4;

	tracker->overlay_layout->adv_offset = 0;
	tracker->overlay_layout->adv_index_offset = 0;
	tracker->overlay_layout->crt_offset = 0;
	tracker->overlay_layout->crt_index_offset = 0;
	tracker->overlay_layout->mutlti_part_adv_i = 0;

	tracker->overlay_layout->max_crt = 0;
	tracker->overlay_layout->max_adv = 0;
	tracker->overlay_layout->scroll_speed = 15;

	tracker->overlay_layout->adv_start_y = (tracker->overlay_layout->padding * 2 + tracker->overlay_layout->crt_size);
	tracker->overlay_layout->max_crt = tracker->o_window_width / (tracker->overlay_layout->crt_size + tracker->overlay_layout->crt_spacing) + 2;
	tracker->overlay_layout->max_adv = tracker->o_window_width / (tracker->overlay_layout->adv_size + tracker->overlay_layout->adv_spacing) + 2;
	tracker->overlay_layout->goals_start_y = 200;
	tracker->overlay_layout->goals_start_x = 30;
	tracker->overlay_layout->goals_spacing = 30;

	return tracker;
}

void tracker_render_main(SDL_Renderer* renderer, FC_Font* font, SDL_Texture* bg_texture, ADV_advancement** advancements, int advancements_n, int window_width, int window_height, MainLayout* l) {
	SDL_Rect rect = { 0, 0, l->adv_size, l->adv_size };
	SDL_Rect blend_rect = { 0, 0, l->adv_size + 8, l->adv_size + l->text_margin + l->spacing_y };
	SDL_Rect criterion_rect = { 0, 0, l->crt_size, l->crt_size };
	SDL_Rect criterion_blend_rect = { 0, 0, 0, l->crt_size };

	check_sdl_code(SDL_SetRenderDrawColor(renderer, 50, 41, 71, 255));
	check_sdl_code(SDL_RenderClear(renderer));

	// RENDER LAYOUT. //
	int offset = 0;
	int crt_offset = 0;
	for (int i = 0; i < advancements_n; ++i) {
		int done = advancements[i]->done;
		int criteria = advancements[i]->criteria_n;

		if (criteria > 0) {
			int max_width = 0;
			int last_y = 0;

			for (int j = 0; j < criteria; ++j) {
				int criterion_done = advancements[i]->criteria[j]->done;
				SDL_Texture* txt = advancements[i]->criteria[j]->texture;

				char* criterion_name = advancements[i]->criteria[j]->name;

				max_width = maxi(FC_GetWidth(font, criterion_name) + l->crt_size + l->crt_text_margin, max_width);
				criterion_rect.y = l->crt_start_y + (l->crt_size + l->crt_spacing_y) * (j % ((window_height - l->crt_start_y - l->padding) / (l->crt_size + l->crt_spacing_y)));

				if (criterion_rect.y < last_y) {
					crt_offset += max_width + l->crt_spacing_x;
					max_width = 0;
				}

				criterion_rect.x = l->padding + crt_offset;
				last_y = criterion_rect.y;

				check_sdl_code(SDL_RenderCopy(renderer, txt, NULL, &criterion_rect));
				FC_Draw(font, renderer, criterion_rect.x + l->crt_size + l->crt_text_margin, criterion_rect.y + l->crt_text_fix, criterion_name);

				if (criterion_done) {
					criterion_blend_rect.w = max_width;
					criterion_blend_rect.x = criterion_rect.x;
					criterion_blend_rect.y = criterion_rect.y;

					check_sdl_code(SDL_SetRenderDrawColor(renderer, 50, 41, 71, 200));
					check_sdl_code(SDL_RenderFillRect(renderer, &criterion_blend_rect));
				}
			}
			crt_offset += max_width + l->crt_group_spacing_x;
			offset += 1;
			continue;
		}

		char* advancement_name = advancements[i]->display_name;

		rect.x = ((i - offset) % (window_width / (l->adv_size + l->spacing_x))) * (l->adv_size + l->spacing_x) + l->padding;
		rect.y = ((i - offset) / (window_width / (l->adv_size + l->spacing_x))) * (l->adv_size + l->spacing_y) + l->padding;

		check_sdl_code(SDL_RenderCopy(renderer, bg_texture, NULL, &(SDL_Rect){rect.x, rect.y, l->adv_size, l->adv_size}));
		check_sdl_code(SDL_RenderCopy(renderer, advancements[i]->texture, NULL, &(SDL_Rect){rect.x + 8, rect.y + 8, 32, 32}));

		int x = rect.x + l->adv_size / 2;
		int y = rect.y + l->adv_size + l->text_margin;
		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, advancement_name);

		if (done) {
			blend_rect.x = rect.x - 4;
			blend_rect.y = rect.y - 2;

			check_sdl_code(SDL_SetRenderDrawColor(renderer, 50, 41, 71, 200));
			check_sdl_code(SDL_RenderFillRect(renderer, &blend_rect));
		}
	}

	SDL_RenderPresent(renderer);
}

void tracker_update_overlay(ADV_advancement** advancements, int advancements_n, int criteria_n, int multi_part_adv, OverlayLayout* l) {
	// Update overlay offsets.
	// if (overlay_render_advancements) {
	l->adv_offset += l->scroll_speed;

	if (l->adv_offset >= l->adv_bg_size + l->adv_spacing) {
		l->adv_offset -= l->adv_bg_size + l->adv_spacing;

		l->adv_index_offset = (l->adv_index_offset + 1) % advancements_n;

		int i = 0;
		while (advancements[l->adv_index_offset]->done == 1 || advancements[l->adv_index_offset]->criteria_n > 0) {
			l->adv_index_offset = (l->adv_index_offset + 1) % advancements_n;
			if (++i > advancements_n) {
				// overlay_render_advancements = 0;
				break;
			}
		}
	}
	// }

	// if (overlay_render_criteria) {
	l->crt_offset += l->scroll_speed;
	if (l->crt_offset >= l->crt_size + l->crt_spacing) {
		l->crt_offset -= l->crt_size + l->crt_spacing;

		if (++l->crt_index_offset >= advancements[l->mutlti_part_adv_i]->criteria_n) {
			l->mutlti_part_adv_i = (l->mutlti_part_adv_i + 1) % multi_part_adv;
			l->crt_index_offset = 0;
		}

		int i = 0;
		while (advancements[l->mutlti_part_adv_i]->criteria[l->crt_index_offset]->done == 1) { // <- Do zmiany teraz
			if (++l->crt_index_offset >= advancements[l->mutlti_part_adv_i]->criteria_n) {
				l->crt_index_offset = 0;
				l->mutlti_part_adv_i = (l->mutlti_part_adv_i + 1) % multi_part_adv;
			}

			if (++i > criteria_n) {
				// overlay_render_criteria = 0;
				break;
			}
		}
	}
	// }
}

void tracker_render_overlay(
	SDL_Renderer* renderer,
	FC_Font* font,
	SDL_Texture* bg_texture,
	SDL_Texture* bg_texture_half_done,
	SDL_Texture* bg_texture_done,
	ADV_advancement** advancements,
	int advancements_n,
	int criteria_n,
	int multi_part_adv,
	Goal** goals,
	int goals_n,
	int window_width,
	int window_height,
	OverlayLayout* l
) {
	SDL_Rect advancement_rect = { 0, l->adv_start_y + (l->adv_bg_size - l->adv_size) / 2, l->adv_size, l->adv_size };
	SDL_Rect advancement_background_rect = { 0, l->adv_start_y, l->adv_bg_size, l->adv_bg_size };
	SDL_Rect criterion_rect = { 0, l->padding, l->crt_size, l->crt_size };

	// OVERLAY WINDOW RENDERING. //
	check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 51, 0, 255));
	check_sdl_code(SDL_RenderClear(renderer));

	// RENDER THE OVERLAY. //
	int multi_part_i = l->mutlti_part_adv_i;
	int rendered_criteria = 0;
	int attempts = 0;
	for (int i = l->crt_index_offset; rendered_criteria < l->max_crt;) {
		ADV_criterion* criterion = advancements[multi_part_i]->criteria[i];

		if (++i >= advancements[multi_part_i]->criteria_n) {
			multi_part_i = (multi_part_i + 1) % multi_part_adv;
			i = 0;
		}

		if (criterion->done == 1) {
			if (++attempts > criteria_n) { break; }

			continue;
		}
		else {
			attempts = 0;
		}

		SDL_Texture* texture = criterion->overlay_texture;

		criterion_rect.x = rendered_criteria * (l->crt_size + l->crt_spacing) - l->crt_offset;
		check_sdl_code(SDL_RenderCopy(renderer, texture, NULL, &criterion_rect));

		++rendered_criteria;
	}

	int rendered_advancements = 0;
	attempts = 0;
	for (int i = l->adv_index_offset; rendered_advancements < l->max_adv; i = (i + 1) % advancements_n) {
		if (advancements[i]->done == 1 || advancements[i]->criteria_n > 0) {
			if (++attempts > criteria_n) { break; }

			continue;
		}
		else {
			attempts = 0;
		}

		SDL_Texture* texture = advancements[i]->overlay_texture;
		char* advancement_name = advancements[i]->display_name;

		advancement_background_rect.x = rendered_advancements * (l->adv_bg_size + l->adv_spacing) - l->adv_offset;
		advancement_rect.x = advancement_background_rect.x + (l->adv_bg_size - l->adv_size) / 2;

		check_sdl_code(SDL_RenderCopy(renderer, bg_texture, NULL, &advancement_background_rect));
		check_sdl_code(SDL_RenderCopy(renderer, texture, NULL, &advancement_rect));

		int x = advancement_background_rect.x + l->adv_bg_size / 2;
		int y = advancement_background_rect.y + l->adv_bg_size + l->text_margin;
		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, advancement_name);

		++rendered_advancements;
	}

	// Render goals.
	for (int k = 0; k < goals_n; ++k) {
		Goal* goal = goals[k];

		int i = 0;
		while (i < goal->sub_goals_n - 1) {
			SubGoal* g = goal->sub_goals[i];
			if (g->progress < g->goal) {
				break;
			}
			++i;
		}

		SubGoal* sub_goal = goal->sub_goals[i];
		char name_buffer[50];
		if (sub_goal->display_type == DISPLAYTYPE_name) {
			strcpy(name_buffer, sub_goal->name);
		} else if (sub_goal->display_type == DISPLAYTYPE_name_progress_goal) {
			snprintf(name_buffer, sizeof name_buffer, "%s\n%d / %d", sub_goal->name, sub_goal->progress, sub_goal->goal);
		} else if (sub_goal->display_type == DISPLAYTYPE_name_progress) {
			snprintf(name_buffer, sizeof name_buffer, "%s%d", sub_goal->name, sub_goal->progress);
		}

		advancement_background_rect.y = l->goals_start_y;
		advancement_rect.y = l->goals_start_y + (l->adv_bg_size - l->adv_size) / 2;

		advancement_background_rect.x = l->goals_start_x + k * (l->adv_bg_size + l->goals_spacing);
		advancement_rect.x = advancement_background_rect.x + (l->adv_bg_size - l->adv_size) / 2;

		if (i > 0 && i < goal->sub_goals_n - 1)
			check_sdl_code(SDL_RenderCopy(renderer, bg_texture_half_done, NULL, &advancement_background_rect));
		else if (i == goal->sub_goals_n - 1)	
			check_sdl_code(SDL_RenderCopy(renderer, bg_texture_done, NULL, &advancement_background_rect));
		else							
			check_sdl_code(SDL_RenderCopy(renderer, bg_texture, NULL, &advancement_background_rect));
		
		check_sdl_code(SDL_RenderCopy(renderer, goals[k]->icon_texture, NULL, &advancement_rect));

		int x = advancement_background_rect.x + l->adv_bg_size / 2;
		int y = advancement_background_rect.y + l->adv_bg_size + l->text_margin;

		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, name_buffer);
	}

	SDL_RenderPresent(renderer);
}

void tracker_delete(Tracker* tracker) {
	if (tracker != NULL) {
		free(tracker->main_layout);
		free(tracker->overlay_layout);
		free(tracker->template_path);
	}
}