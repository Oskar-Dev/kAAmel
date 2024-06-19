#include "../include/cJSON.h"
#include "goal.h"
#include "utils.h"

#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

Goal** goal_init(const int n) {
	Goal** goals = malloc(n * sizeof *goals);
	if (goals == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for goals.\n");
		exit(1);
	}

	return goals;
}

SubGoal* goal_sub_create(const SubGoalType type, const int display_count, const char* name, const char* root_name, const int goal) {
	SubGoal* sub_goal = malloc(sizeof *sub_goal);
	if (sub_goal == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for a sub-goal.\n");
		exit(1);
	}

	sub_goal->name = malloc(strlen(name) * sizeof(char));
	if (sub_goal->name == NULL) {
		free(sub_goal);
		printf("[MEMORY ERROR] Couldn't allocate enough memory for sub-goal's name.\n");
		exit(1);
	}

	sub_goal->root_name = malloc(strlen(root_name) * sizeof(char));
	if (sub_goal->root_name == NULL) {
		free(sub_goal->name);
		free(sub_goal);
		printf("[MEMORY ERROR] Couldn't allocate enough memory for sub-goal's root name.\n");
		exit(1);
	}

	strcpy(sub_goal->name, name);
	strcpy(sub_goal->root_name, root_name);
	sub_goal->goal = goal;
	sub_goal->progress = 0;
	sub_goal->type = type;
	sub_goal->display_count = display_count;

	return sub_goal;
}

Goal* goal_create(const SDL_Renderer* renderer, const GoalType type) {
	Goal* goal = malloc(sizeof * goal);
	if (goal == NULL) {
		goto memory_error;
	}

	goal->done = 0;

	switch (type) {
	case GOALTYPE_nautilus_shells:
		goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/nautilus_shell.png"));
		
		goal->sub_goals_n = 4;
		goal->sub_goals = malloc(goal->sub_goals_n * sizeof *goal->sub_goals);
		if (goal->sub_goals == NULL) {
			goto memory_error;
		}

		// goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_advancement, 0, "Thunder xd", "minecraft:adventure/very_very_frightening", 1);
		goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, 1, "Shells", "minecraft:nautilus_shell", 8);
		goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_item_craft, 0, "Craft Conduit", "minecraft:conduit", 1);
		goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_advancement, 0, "Do HDWGH", "minecraft:nether/all_effects", 1);
		goal->sub_goals[3] = goal_sub_create(SUBGOALTYPE_final, 0, "Done with HDWGH", "", 0);
		// goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_item_pick_up, "PICKED UP Conduit", "minecraft:conduit", 2);

		break;

	case GOALTYPE_trident:
		goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/trident.png"));

		goal->sub_goals_n = 3;
		goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
		if (goal->sub_goals == NULL) {
			goto memory_error;
		}

		goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, 0, "Obtain Trident", "minecraft:trident", 1);
		goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_advancement, 0, "Awaiting Thunder", "minecraft:adventure/very_very_frightening", 1);
		goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_final, 0, "Done With Thunder", "", 0);

		break;
	
	default:
		return NULL;
	}

	return goal;

memory_error:
	printf("[MEMORY ERROR] Couldn't allocate enough memory to create a new goal.\n");	
	exit(1);
}

void goal_update(Goal** goals, const int goals_n, const ADV_advancement** adv, const int adv_n, const char* file_path) {
	// DO ZROBIENIA: Zrób aby aktualizowa³o siê przy weœciu na nowy œwiat bez tworzenia niczego oraz bez podnodnoszenia ¿adnych przedmiotów.
	cJSON* data = cJSON_from_file(file_path);
	if (!data) {
		return;
	}

	cJSON* stats = cJSON_GetObjectItemCaseSensitive(data, "stats");
	if (!stats) {
		return;
	}

	cJSON* picked_up = cJSON_GetObjectItemCaseSensitive(stats, "minecraft:picked_up");
	if (!picked_up) {
		return;
	}

	cJSON* crafted = cJSON_GetObjectItemCaseSensitive(stats, "minecraft:crafted");
	if (!picked_up) {
		return;
	}

	for (int i = 0; i < goals_n; ++i) {
		Goal* goal = goals[i];
		// if (goal->done) continue;
		int sub_goals_n = goal->sub_goals_n;

		for (int j = 0; j < sub_goals_n; ++j) {
			SubGoal* sub_goal = goal->sub_goals[j];
			
			if (sub_goal->progress >= sub_goal->goal) {
				if (j + 1 == sub_goals_n) {
					goal->done = 1;
				}

				// continue;
			}

			switch (sub_goal->type) {
				case SUBGOALTYPE_item_pick_up: {
					cJSON* value = cJSON_GetObjectItemCaseSensitive(picked_up, sub_goal->root_name);
					if (value && value->valueint) {
						sub_goal->progress = value->valueint;
					} else {
						sub_goal->progress = 0;
						goal->done = 0;
					}
					break;
				}
			
				case SUBGOALTYPE_item_craft: {
					cJSON* value = cJSON_GetObjectItemCaseSensitive(crafted, sub_goal->root_name);
					if (value && value->valueint) {
						sub_goal->progress = value->valueint;
					} else {
						sub_goal->progress = 0;
						goal->done = 0;
					}
					break;
				}

				case SUBGOALTYPE_advancement: {
					for (int i = 0; i < adv_n; ++i) {
						if (strcmp(adv[i]->root_name, sub_goal->root_name) == 0) {
							sub_goal->progress = adv[i]->done;
						}
					}
					break;
				}
			
				default:
					break;
			}
		}
	}

	cJSON_Delete(data);
}