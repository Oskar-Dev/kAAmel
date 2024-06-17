#include "tracker.h"

Tracker* create_tracker(Version version, Tracker *tracker) {
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
		multi_part_advancements = 7;
		m_window_width = 1300;
		m_window_height = 768;
		o_window_width = 1600;
		o_window_height = 250;

		tracker->template_path = malloc((strlen("resources/templates/1.16/advancements_optimised.json") + 1) * sizeof(char));
		if (tracker->template_path == NULL) {
			printf("[MEMORY ERROR] Couldn't allocate enough space for application's template path.\n");
		}

		strcpy(tracker->template_path, "resources/templates/1.16/advancements_optimised.json");
		break;

	case VERSION_1_20_6:
		advancements = 69;
		criteria = 185;
		multi_part_advancements = 8;
		m_window_width = 1630;
		m_window_height = 768;
		o_window_width = 1600;
		o_window_height = 250;

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
		o_window_height = 250;

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
	tracker->main_layout = malloc(sizeof *tracker->main_layout);
	if (tracker->main_layout == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for the main layout.\n");
	}

	tracker->main_layout->adv_size				= 48;
	tracker->main_layout->padding				= 8;
	tracker->main_layout->spacing_x				= 14;
	tracker->main_layout->spacing_y				= 20;
	tracker->main_layout->text_margin			= 4;
	tracker->main_layout->box_width				= 2;
	tracker->main_layout->crt_start_y			= 220;
	tracker->main_layout->crt_size				= 16;
	tracker->main_layout->crt_spacing_y			= 8;
	tracker->main_layout->crt_text_margin		= 4;
	tracker->main_layout->crt_text_fix			= 3;
	tracker->main_layout->crt_max_width			= 126;

	tracker->overlay_layout = malloc(sizeof * tracker->overlay_layout);
	if (tracker->overlay_layout == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for the main layout.\n");
	}

	tracker->overlay_layout->padding			= 10;
	tracker->overlay_layout->crt_size			= 48;
	tracker->overlay_layout->crt_spacing		= 10;
	tracker->overlay_layout->adv_start_y		= 0;
	tracker->overlay_layout->adv_size			= 64;
	tracker->overlay_layout->adv_bg_size		= 80;
	tracker->overlay_layout->adv_spacing		= 32;

	tracker->overlay_layout->text_margin		= 4;

	tracker->overlay_layout->adv_offset			= 0;
	tracker->overlay_layout->adv_index_offset	= 0;
	tracker->overlay_layout->crt_offset			= 0;
	tracker->overlay_layout->crt_index_offset	= 0;
	tracker->overlay_layout->mutlti_part_adv_i	= 0;

	tracker->overlay_layout->max_crt			= 0;
	tracker->overlay_layout->max_adv			= 0;
	tracker->overlay_layout->scroll_speed		= 5;

	tracker->overlay_layout->adv_start_y		= (tracker->overlay_layout->padding * 2 + tracker->overlay_layout->crt_size);
	tracker->overlay_layout->max_crt			= tracker->o_window_width / (tracker->overlay_layout->crt_size + tracker->overlay_layout->crt_spacing) + 2;
	tracker->overlay_layout->max_adv			= tracker->o_window_width / (tracker->overlay_layout->adv_size + tracker->overlay_layout->adv_spacing) + 2;

	return tracker;
}

void tracker_render_main(SDL_Renderer* renderer, FC_Font* font, ADV_advancement** advancements, int advancements_n, int window_width, int window_height, MainLayout* l) {
	SDL_Rect rect = { 0, 0, l->adv_size, l->adv_size };
	SDL_Rect blend_rect = { 0, 0, l->adv_size + 8, l->adv_size + l->text_margin + l->spacing_y };
	SDL_Rect criterion_rect = { 0, 0, l->crt_size, l->crt_size };
	SDL_Rect criterion_blend_rect = { 0, 0, l->crt_max_width, l->crt_size };

	check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
	check_sdl_code(SDL_RenderClear(renderer));

	// RENDER LAYOUT. //
	int offset = 0;
	int criteria_offset = 0;
	for (int i = 0; i < advancements_n; ++i) {
		int done = advancements[i]->done;
		int criteria = advancements[i]->criteria_n;

		if (criteria > 0) {
			for (int j = 0; j < criteria; ++j) {
				int criterion_done = advancements[i]->criteria[j]->done;
				SDL_Texture* txt = advancements[i]->criteria[j]->texture;

				char* criterion_name = advancements[i]->criteria[j]->name;
				criterion_rect.x = l->padding + l->crt_max_width * offset + l->crt_max_width * (criteria_offset + j / ((window_height - l->crt_start_y - l->padding) / (l->crt_size + l->crt_spacing_y)));
				criterion_rect.y = l->crt_start_y + (l->crt_size + l->crt_spacing_y) * (j % ((window_height - l->crt_start_y - l->padding) / (l->crt_size + l->crt_spacing_y)));

				check_sdl_code(SDL_RenderCopy(renderer, txt, NULL, &criterion_rect));
				FC_Draw(font, renderer, criterion_rect.x + l->crt_size + l->crt_text_margin, criterion_rect.y + l->crt_text_fix, criterion_name);

				if (criterion_done) {
					criterion_blend_rect.x = criterion_rect.x;
					criterion_blend_rect.y = criterion_rect.y;

					check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200));
					check_sdl_code(SDL_RenderFillRect(renderer, &criterion_blend_rect));
				}
			}

			criteria_offset += criteria / ((window_height - l->crt_start_y - l->padding - l->crt_size) / (l->crt_size + l->crt_spacing_y));
			offset += 1;
			continue;
		}

		char* advancement_name = advancements[i]->display_name;

		rect.x = ((i - offset) % (window_width / (l->adv_size + l->spacing_x))) * (l->adv_size + l->spacing_x) + l->padding;
		rect.y = ((i - offset) / (window_width / (l->adv_size + l->spacing_x))) * (l->adv_size + l->spacing_y) + l->padding;

		// render_advancement_box(renderer, rect.x - main_advancement_box_width, rect.y - main_advancement_box_width, size + 2 * main_advancement_box_width, main_advancement_box_width);
		check_sdl_code(SDL_RenderCopy(renderer, advancements[i]->texture, NULL, &(SDL_Rect){rect.x + 8, rect.y + 8, 32, 32}));

		int x = rect.x + l->adv_size / 2;
		int y = rect.y + l->adv_size + l->text_margin;
		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, advancement_name);

		if (done) {
			blend_rect.x = rect.x - 4;
			blend_rect.y = rect.y - 2;

			check_sdl_code(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200));
			check_sdl_code(SDL_RenderFillRect(renderer, &blend_rect));
		}
	}

	SDL_RenderPresent(renderer);
}

void tracker_update_overlay(ADV_advancement** advancements, int advancements_n, int criteria_n, int multi_part_adv, OverlayLayout* l) {
	// Update overlay offsets.
	// if (overlay_render_advancements) {
	l->adv_offset += l->scroll_speed;

	if (l->adv_offset >= l->adv_size + l->adv_spacing) {
		l->adv_offset -= l->adv_size + l->adv_spacing;

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
			if (++ l->crt_index_offset >= advancements[l->mutlti_part_adv_i]->criteria_n) {
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
	SDL_Texture* background_texture,
	ADV_advancement** advancements,
	int advancements_n,
	int criteria_n,
	int multi_part_adv,
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
	check_sdl_code(SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));

	// RENDER THE OVERLAY. //
	// if (overlay_render_criteria) {
	int multi_part_i = l->mutlti_part_adv_i;
	int rendered_criteria = 0;
	int attempts = 0;
	for (int i = l->crt_index_offset; rendered_criteria < l->max_crt;) {
		ADV_criterion* criterion = advancements[multi_part_i]->criteria[i];

		if (++i >= advancements[multi_part_i]->criteria_n) {
			multi_part_i = (multi_part_i + 1) % multi_part_adv;
			i = 0;
		}

		if (criterion->done == 1) { // <- Do zmiany teraz.
			if (++attempts > criteria_n) { break; }

			continue;
		}
		else {
			attempts = 0;
		}

		SDL_Texture* texture = criterion->overlay_texture; // <- Do zmiany teraz.

		criterion_rect.x = rendered_criteria * (l->crt_size + l->crt_spacing) - l->crt_offset;
		check_sdl_code(SDL_RenderCopy(renderer, texture, NULL, &criterion_rect));

		++rendered_criteria;
	}
	// }

	// if (overlay_render_advancements) {
	int rendered_advancements = 0;
	// int attempts = 0;
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

		advancement_rect.x = rendered_advancements * (l->adv_size + l->adv_spacing) - l->adv_offset;
		advancement_background_rect.x = advancement_rect.x + (l->adv_size - l->adv_bg_size) / 2;

		// render_advancement_box(overlay_renderer, overlay_advancement_rect.x, overlay_advancements_start_y, overlay_advancement_size, overlay_advancement_box_width);
		check_sdl_code(SDL_RenderCopy(renderer, background_texture, NULL, &advancement_background_rect));
		check_sdl_code(SDL_RenderCopy(renderer, texture, NULL, &advancement_rect));

		int x = advancement_background_rect.x + l->adv_bg_size / 2;
		int y = advancement_background_rect.y + l->adv_bg_size + l->text_margin;
		FC_DrawAlign(font, renderer, x, y, FC_ALIGN_CENTER, advancement_name);

		++rendered_advancements;
	}
	// }

	SDL_RenderPresent(renderer);
}

void delete_tracker(Tracker* tracker) {
	if (tracker != NULL) {
		free(tracker->main_layout);
		free(tracker->overlay_layout);
		free(tracker->template_path);
		// free(tracker);
	}
}