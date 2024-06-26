#pragma once

#ifndef TRACKER_H
#define TRACKER_H

#include <SDL.h>

#include "ADV.h"
#include "goal.h"
#include "../include/SDL_FontCache.h"

typedef	enum {
	VERSION_1_16,
	VERSION_1_20_6,
	VERSION_1_21,
} Version;

typedef struct {
	int adv_size;
	int padding;
	int spacing_x;
	int spacing_y;
	int text_margin;
	int box_width;
	int crt_start_y;
	int crt_size;
	int crt_spacing_y;
	int crt_spacing_x;
	int crt_group_spacing_x;
	int crt_text_margin;
	int crt_text_fix;
} MainLayout;

typedef struct {
	int padding;
	int crt_size;
	int crt_spacing;
	int adv_start_y;
	int adv_size;
	int adv_bg_size;
	int adv_spacing;
	int text_margin;
	int adv_offset;
	int adv_index_offset;
	int crt_offset;
	int crt_index_offset;
	int mutlti_part_adv_i;
	int goals_start_y;
	int goals_start_x;
	int goals_spacing;

	int max_crt;
	int max_adv;
	int scroll_speed;
} OverlayLayout;

typedef struct {
	Version version;
	char* saves_path;
} Settings;

typedef struct {
	int advancements;
	int criteria;
	int multi_part_advancements;
	int m_window_width;
	int m_window_height;
	int o_window_width;
	int o_window_height;
	char* template_path;

	Settings* settings;
	MainLayout* main_layout;
	OverlayLayout* overlay_layout;
} Tracker;

Tracker* tracker_create(Tracker* tracker);
void tracker_create_default_settings(Settings* settings);
int tracker_load_settings(Settings* settings);
void tracker_render_main(SDL_Renderer* renderer, FC_Font* font, SDL_Texture* bg_texture, ADV_advancement** advancements, int advancements_n, int window_width, int window_height, MainLayout* l);
void tracker_update_overlay(ADV_advancement** advancements, int advancements_n, int criteria_n, int multi_part_adv, OverlayLayout* l);
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
);
void tracker_delete(Tracker* tracker);

#endif