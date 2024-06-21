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

SubGoal* goal_sub_create(const SubGoalType type, const char* name, const char* root_name, const int display_count, const int goal) {
	SubGoal* sub_goal = malloc(sizeof *sub_goal);
	if (sub_goal == NULL) {
		printf("[MEMORY ERROR] Couldn't allocate enough memory for a sub-goal.\n");
		exit(1);
	}

	sub_goal->name = malloc((strlen(name) + 1) * sizeof(char));
	if (sub_goal->name == NULL) {
		free(sub_goal);
		printf("[MEMORY ERROR] Couldn't allocate enough memory for sub-goal's name.\n");
		exit(1);
	}

	sub_goal->root_name = malloc((strlen(root_name) + 1) * sizeof(char));
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
		
			goal->sub_goals_n = 5;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof *goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, "Shells", "minecraft:nautilus_shell", 1, 8);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_item_craft, "Craft\nConduit", "minecraft:conduit", 0, 1);
			goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_item_use, "Place\nConduit", "minecraft:conduit", 0, 1);
			goal->sub_goals[3] = goal_sub_create(SUBGOALTYPE_advancement, "Do HDWGH", "minecraft:nether/all_effects", 0, 1);
			goal->sub_goals[4] = goal_sub_create(SUBGOALTYPE_final, "Done with\nHDWGH", "", 0, 0);
			break;

		case GOALTYPE_trident:
			goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/trident.png"));
			
			goal->sub_goals_n = 4;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, "Obtain\nTrident", "minecraft:trident", 0, 1);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_advancement, "Awaiting\nThunder", "minecraft:adventure/very_very_frightening", 0, 1);
			goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_advancement, "Awaiting\nThunder", "minecraft:adventure/lightning_rod_with_villager_no_fire", 0, 1);
			goal->sub_goals[3] = goal_sub_create(SUBGOALTYPE_final, "Done With\nThunder", "", 0, 0);
			break;

		case GOALTYPE_wither_skulls:
			goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/wither_skeleton_skull.png"));

			goal->sub_goals_n = 2;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, "Skulls", "minecraft:wither_skeleton_skull", 1, 3);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_final, "Done With\nSkulls", "", 0, 0);
			break;

		case GOALTYPE_heavy_core:
			goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/mace.png"));

			goal->sub_goals_n = 4;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, "Obtain\nHeavy Core", "minecraft:heavy_core", 0, 1);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_item_craft, "Craft\nMace", "minecraft:mace", 0, 1);
			goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_advancement, "Do\nOver-Overkill", "minecraft:adventure/overoverkill", 0, 1);
			goal->sub_goals[3] = goal_sub_create(SUBGOALTYPE_final, "Done With\nMace", "", 0, 0);
			break;

		case GOALTYPE_sniffers:
			goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/sniffer_egg.png"));

			goal->sub_goals_n = 4;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_item_pick_up, "Eggs", "minecraft:sniffer_egg", 1, 2);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_item_use, "Place Eggs", "minecraft:sniffer_egg", 0, 2);
			goal->sub_goals[2] = goal_sub_create(SUBGOALTYPE_advancement, "Feed Snifflet", "minecraft:husbandry/feed_snifflet", 0, 1);
			goal->sub_goals[3] = goal_sub_create(SUBGOALTYPE_final, "Done With\nSniffers", "", 0, 0);
			break;

		case GOALTYPE_silence:
			goal->icon_texture = check_sdl_ptr(IMG_LoadTexture(renderer, "resources/sprites/items/silence_trim.png"));

			goal->sub_goals_n = 2;
			goal->sub_goals = malloc(goal->sub_goals_n * sizeof * goal->sub_goals);
			if (goal->sub_goals == NULL) {
				goto memory_error;
			}

			goal->sub_goals[0] = goal_sub_create(SUBGOALTYPE_advancement, "Obtain\nSilence", "minecraft:recipes/misc/silence_armor_trim_smithing_template", 0, 1);
			goal->sub_goals[1] = goal_sub_create(SUBGOALTYPE_final, "Silence\nObtained", "", 0, 0);
			break;
	
		default:
			return NULL;
	}

	// minecraft:recipes/misc/silence_armor_trim_smithing_template

	return goal;

memory_error:
	printf("[MEMORY ERROR] Couldn't allocate enough memory to create a new goal.\n");	
	exit(1);
}

void goal_update(Goal** goals, const int goals_n, const char* file_path) {
	// DO ZROBIENIA: Zrób aby aktualizowa³o siê przy weœciu na nowy œwiat bez tworzenia niczego oraz bez podnodnoszenia ¿adnych przedmiotów.
	// DO ZROBIENIA: Mo¿e dodaj "display_if_not_done" albo coœ podobnego?
	// DO ZROBIENIA: Nie renderuj tego co ka¿d¹ klatkê.
	cJSON* data = cJSON_from_file(file_path);
	if (!data) {
		return;
	}

	cJSON* stats = cJSON_GetObjectItemCaseSensitive(data, "stats");
	// if (!stats) {
	// 	return;
	// }

	cJSON* picked_up = cJSON_GetObjectItemCaseSensitive(stats, "minecraft:picked_up");
	// if (!picked_up) {
	// 	return;
	// }

	cJSON* crafted = cJSON_GetObjectItemCaseSensitive(stats, "minecraft:crafted");
	// if (!crafted) {
	// 	return;
	// }

	cJSON* used = cJSON_GetObjectItemCaseSensitive(stats, "minecraft:used");
	// if (!used) {
	// 	return;
	// }

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

				case SUBGOALTYPE_item_use: {
					cJSON* value = cJSON_GetObjectItemCaseSensitive(used, sub_goal->root_name);
					if (value && value->valueint) {
						sub_goal->progress = value->valueint;
					} else {
						sub_goal->progress = 0;
						goal->done = 0;
					}
					break;
				}

				/*case SUBGOALTYPE_advancement: { // ADV_update_advancements handles the advancement case.
					for (int i = 0; i < adv_n; ++i) {
						if (strcmp(adv[i]->root_name, sub_goal->root_name) == 0) {
							sub_goal->progress = adv[i]->done;
						}
					}
					break;
				}*/
			
				default:
					break;
			}
		}
	}

	cJSON_Delete(data);
}

void goal_delete(Goal** goals, const int goals_n) {
	for (int i = 0; i < goals_n; ++i) {
		Goal* goal = goals[i];
		SDL_DestroyTexture(goal->icon_texture);

		for (int j = 0; j < goal->sub_goals_n; ++j) {
			SubGoal* sub_goal = goal->sub_goals[j];
			free(sub_goal->name);
			free(sub_goal->root_name);
		}

		free(goal->sub_goals);
		free(goal);
	}

	free(goals);
}