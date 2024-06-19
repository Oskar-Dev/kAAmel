#pragma once

#ifndef GOAL_H
#define GOAL_H

#include <SDL.h>

#include "ADV.h"

typedef enum {
	SUBGOALTYPE_final,
	SUBGOALTYPE_advancement,
	SUBGOALTYPE_criterion,
	SUBGOALTYPE_item_pick_up,
	SUBGOALTYPE_item_craft,
} SubGoalType;

typedef struct {
	char* name;
	char* root_name;
	SubGoalType type;
	int display_count;
	
	int progress;
	int goal;
} SubGoal;

typedef enum {
	GOALTYPE_nautilus_shells,
	GOALTYPE_trident
} GoalType;

typedef struct {
	int done;
	int sub_goals_n;
	SubGoal** sub_goals;
	SDL_Texture* icon_texture;
} Goal;

Goal** goal_init(const int n);
void goal_update(Goal** goals, const int goals_n, const ADV_advancement** adv, const int adv_n, const char* file_path);
Goal* goal_create(const SDL_Renderer* renderer, const GoalType type);
void goal_update(Goal** goals, const char* stats_path);

#endif