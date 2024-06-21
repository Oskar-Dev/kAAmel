#pragma once

#ifndef GOAL_H
#define GOAL_H

#include <SDL.h>

typedef enum {
	DISPLAYTYPE_name,
	DISPLAYTYPE_name_progress_goal,
	DISPLAYTYPE_name_progress,
} DisplayType;

typedef enum {
	SUBGOALTYPE_final,
	SUBGOALTYPE_advancement,
	SUBGOALTYPE_criterion,
	SUBGOALTYPE_item_pick_up,
	SUBGOALTYPE_item_craft,
	SUBGOALTYPE_item_use,
	SUBGOALTYPE_kill,
} SubGoalType;

typedef struct {
	char* name;
	char* root_name;
	SubGoalType type;
	DisplayType display_type;
	int display_if_not_done;
	
	int progress;
	int goal;
} SubGoal;

typedef enum {
	GOALTYPE_nautilus_shells,
	GOALTYPE_trident,
	GOALTYPE_wither_skulls,
	GOALTYPE_heavy_core,
	GOALTYPE_sniffers,
	GOALTYPE_silence,
} GoalType;

typedef struct {
	int done;
	int sub_goals_n;
	SubGoal** sub_goals;
	SDL_Texture* icon_texture;
} Goal;

Goal** goal_init(const int n);
SubGoal* goal_sub_create(const SubGoalType type, const char* name, const char* root_name, const DisplayType display_type, const int goal);
Goal* goal_create(const SDL_Renderer* renderer, const GoalType type);
void goal_update(Goal** goals, const int goals_n, const char* file_path);
void goal_delete(Goal** goals, const int goals_n);

#endif