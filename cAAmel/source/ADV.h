#pragma once

#ifndef ADV_H
#define ADV_H

#include "../include/cJSON.h"
#include <SDL.h>

typedef struct {
	char* name;
	char* icon;
	char* root_name;
	int done;
	SDL_Texture* texture;
	SDL_Texture* overlay_texture;
} ADV_criterion;

typedef struct { 
	char* name;
	char* display_name;
	char* icon;
	char* root_name;
	int done;
	int criteria_n;
	ADV_criterion** criteria;
	SDL_Texture* texture;
	SDL_Texture* overlay_texture;
} ADV_advancement;

cJSON* ADV_get_json(char* file_path);

ADV_criterion* ADV_new_criterion(char* name, char* icon, char* root_name, int done);
void ADV_delete_criterion(ADV_criterion* criterion);

ADV_advancement* ADV_new_advancement(char* name, char* display_name, char* icon, char* root_name, ADV_criterion** criteria, int criteria_n, int done);
void ADV_delete_advancement(ADV_advancement* advancement);

ADV_advancement** ADV_object_from_template(cJSON* template, int n);
void ADV_update_advancements(ADV_advancement** advancements, int n, char* path);

#endif

