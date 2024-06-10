#pragma once

#ifndef ADV_H
#define ADV_H

#include "cJSON.h"

typedef struct {
	char* name;
	char* icon;
	char* root_name;
	int done;
} ADV_criterion;

typedef struct { 
	char* name;
	char* display_name;
	char* icon;
	char* root_name;
	int done;
	int criteria_n;
	ADV_criterion** criteria;
} ADV_advancement;

ADV_criterion* ADV_new_criterion(char* name, char* icon, char* root_name, int done);
void ADV_delete_criterion(ADV_criterion* criterion);

ADV_advancement* ADV_new_advancement(char* name, char* display_name, char* icon, char* root_name, ADV_criterion** criteria, int criteria_n, int done);
void ADV_delete_advancement(ADV_advancement* advancement);

cJSON* ADV_get_json(char* file_path);
ADV_advancement** ADV_object_from_template(cJSON* template, int n);
void ADV_update_advancements(ADV_advancement** advancements, int n, char* path);

#endif

