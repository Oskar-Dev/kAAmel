#pragma once

#ifndef GOAL_H
#define GOAL_H

#include <SDL.h>

typedef enum {
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
} GoalType;

typedef struct {
	int done;
	int sub_goals_n;
	SubGoal** sub_goals;
	SDL_Texture* icon_texture;
} Goal;

Goal** goal_init(const int n);
SubGoal* goal_sub_create(const SubGoalType type, const char* name, const char* root_name, const int goal);
Goal* goal_create(const SDL_Renderer* renderer, const GoalType type);
void goal_update(Goal** goals, const char* stats_path);

#endif