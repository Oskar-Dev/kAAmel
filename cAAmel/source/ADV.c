#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adv.h"

cJSON* ADV_get_json(char* file_path) {
	char* buffer = NULL;
	FILE* f;

	if (fopen_s(&f, file_path, "r") != 0) {
		goto error;
	}

	if (fseek(f, 0, SEEK_END) < 0) {
		goto error;
	}

	long m = ftell(f);
	if (m < 0) {
		goto error;
	}

	buffer = malloc(sizeof(char) * m);
	if (buffer == NULL) {
		goto error;
	}

	if (fseek(f, 0, SEEK_SET) < 0) {
		goto error;
	}

	size_t n = fread(buffer, 1, m, f);

	if (ferror(f)) {
		goto error;
	}

	fclose(f);

	cJSON* json = cJSON_Parse(buffer);
	if (json == NULL) {
		cJSON_Delete(json);
		goto error;
	}

	free(buffer);

	return json;
error:
	if (f) {
		fclose(f);
	}

	if (buffer) {
		free(buffer);
	}

	printf("[FILE ERROR] Couldn't open the file: %s\n", file_path);
	return NULL;
}

ADV_criterion* ADV_new_criterion(char* name, char* icon, char* root_name, int done) {
	ADV_criterion* criterion = malloc(sizeof *criterion);
	if (criterion == NULL) {
		return NULL;
	}

	criterion->name = malloc(sizeof(char) * (1+strlen(name)));
	if (name == NULL) {
		free(criterion);
		return NULL;
	}

	criterion->icon = malloc(sizeof(char) * (1 + strlen(icon)));
	if (name == NULL) {
		free(criterion->name);
		free(criterion);
		return NULL;
	}

	criterion->root_name = malloc(sizeof(char) * (1 + strlen(root_name)));
	if (name == NULL) {
		free(criterion->name);
		free(criterion->icon);
		free(criterion);
		return NULL;
	}

	criterion->name = name;
	criterion->icon = icon;
	criterion->root_name = root_name;
	criterion->done = done;

	return criterion;
}

void ADV_delete_criterion(ADV_criterion* criterion) {
	if (criterion != NULL) {
		free(criterion->name);
		free(criterion->icon);
		free(criterion->root_name);
		free(criterion);
	}
}

ADV_advancement* ADV_new_advancement(char* name, char* display_name, char* icon, char* root_name, ADV_criterion** criteria, int criteria_n, int done) {
	ADV_advancement* advancement = malloc(sizeof *advancement);
	if (advancement == NULL) {
		return NULL;
	};

	advancement->name = malloc(sizeof(char) * (1+strlen(name)));
	if (advancement->name == NULL) {
		free(advancement);
		return NULL;
	}

	advancement->display_name = malloc(sizeof(char) * (1 + strlen(display_name)));
	if (advancement->display_name == NULL) {
		free(advancement->name);
		free(advancement);
		return NULL;
	}

	advancement->icon = malloc(sizeof(char) * (1 + strlen(icon)));
	if (advancement->icon == NULL) {
		free(advancement->name);
		free(advancement->display_name);
		free(advancement);
		return NULL;
	}

	advancement->root_name = malloc(sizeof(char) * (1 + strlen(root_name)));
	if (advancement->icon == NULL) {
		free(advancement->name);
		free(advancement->display_name);
		free(advancement->icon);
		free(advancement);
		return NULL;
	}

	if (criteria_n > 0) {
		advancement->criteria = malloc(criteria_n * sizeof *advancement->criteria);
		if (advancement->criteria == NULL) {
			free(advancement->name);
			free(advancement->display_name);
			free(advancement->icon);
			free(advancement->root_name);
			free(advancement);
			return NULL;
		}
	}

	//strcpy_s(advancement->name, name, sizeof(advancement->name));
	//strcpy_s(advancement->display_name, display_name, sizeof(advancement->display_name));
	//strcpy_s(advancement->icon, icon, sizeof(advancement->icon));
	advancement->name = name;
	advancement->display_name = display_name;
	advancement->icon = icon;
	advancement->root_name = root_name;
	advancement->criteria_n = criteria_n;
	
	if (criteria_n > 0) {
		advancement->criteria = criteria;
	}

	advancement->done = 0;

	return advancement;
}

void ADV_delete_advancement(ADV_advancement* advancement) {
	if (advancement != NULL) {
		free(advancement->name);
		free(advancement->display_name);
		free(advancement->icon);
		free(advancement->root_name);

		for (int i = 0; i < advancement->criteria_n; ++i) {
			ADV_delete_criterion(advancement->criteria[i]);
		}

		free(advancement);
	}
}

ADV_advancement** ADV_object_from_template(cJSON* template, int n) {
	cJSON* entry = template->child;
	
	ADV_advancement** advancements = malloc(n * sizeof *advancements); 
	if (advancements == NULL) {
		goto memory_error;
	}

	for (int i = 0; entry; ++i) {
		// cJSON* done = cJSON_GetObjectItemCaseSensitive(entry, "done");
		cJSON* name = cJSON_GetObjectItemCaseSensitive(entry, "name");
		cJSON* display_name = cJSON_GetObjectItemCaseSensitive(entry, "displayName");
		cJSON* icon = cJSON_GetObjectItemCaseSensitive(entry, "icon");
		cJSON* root_name = cJSON_GetObjectItemCaseSensitive(entry, "rootName");
		cJSON* criteria = cJSON_GetObjectItemCaseSensitive(entry, "criteria");
		cJSON* criteria_number = cJSON_GetObjectItemCaseSensitive(entry, "criteriaNumber");
		
		if (!name || !display_name || !icon || !root_name || !criteria || !criteria_number || !root_name->valuestring || !name->valuestring || !display_name->valuestring || !icon->valuestring) {
			goto error;
		}
		
		// int done_ = 0;
		// if (done && cJSON_IsTrue(done)) {
		// 	done_ = 1;
		// }

		int criteria_n = 0;
		ADV_criterion** criteria_a = NULL;
		cJSON* criteria_entry = criteria->child;

		if (criteria_entry) {
			criteria_a = malloc(criteria_number->valueint * sizeof *criteria_a);
			if (criteria_a == NULL) {
				goto memory_error;
			}
			
			while (criteria_entry) {
				// cJSON* criterion_done = cJSON_GetObjectItemCaseSensitive(criteria_entry, "done");
				cJSON* criterion_name = cJSON_GetObjectItemCaseSensitive(criteria_entry, "name");
				cJSON* criterion_icon = cJSON_GetObjectItemCaseSensitive(criteria_entry, "icon");
				cJSON* criterion_root_name = cJSON_GetObjectItemCaseSensitive(criteria_entry, "rootName");

				if (!criterion_name || !criterion_icon || !criterion_root_name || 
					!criterion_name->valuestring || !criterion_icon->valuestring || !criterion_root_name->valuestring
				) {
					goto error;
				}

				criteria_a[criteria_n] = ADV_new_criterion(
					_strdup(criterion_name->valuestring),
					_strdup(criterion_icon->valuestring),
					_strdup(criterion_root_name->valuestring),
					0
				);

				++criteria_n;
				criteria_entry = criteria_entry->next;
			}
		}

		advancements[i] = ADV_new_advancement(
			_strdup(name->valuestring), 
			_strdup(display_name->valuestring),
			_strdup(icon->valuestring),
			_strdup(root_name->valuestring),
			criteria_a,
			criteria_n,
			0
		);

		entry = entry->next;
		continue;

	error:
		printf("[TEMPLATE ERROR]: Inavlid template file.\n");

		for (int j = 0; j < i; ++j) {
			ADV_delete_advancement(advancements[j]);
		}

		free(advancements);

		return NULL;
	}

	return advancements;

memory_error:
	printf("[MEMORY ERROR]: Couldn't allocate enough space for the advancements array.\n");
	return NULL;
}

void ADV_update_advancements(ADV_advancement** advancements, int n, char* path) {
	cJSON* data = ADV_get_json(path);
	if (data == NULL) {
		goto error;
	}

	for (int i = 0; i < n; ++i) {
		char* root_name = advancements[i]->root_name;
		int criteria_number = advancements[i]->criteria_n;
		ADV_criterion** criteria_to_update = advancements[i]->criteria;

		cJSON* entry = cJSON_GetObjectItemCaseSensitive(data, root_name);
		if (entry) {
			cJSON* done = cJSON_GetObjectItemCaseSensitive(entry, "done");
			cJSON* criteria = cJSON_GetObjectItemCaseSensitive(entry, "criteria");

			if (!done || !criteria) goto error;

			if (cJSON_IsTrue(done)) {
				advancements[i]->done = 1;
			}

			if (criteria_number > 0) {
				for (int j = 0; j < criteria_number; ++j) {
					char* criterion_root_name = criteria_to_update[j]->root_name;
					cJSON* criterion_entry = cJSON_GetObjectItemCaseSensitive(criteria, criterion_root_name);

					if (criterion_entry) {
						criteria_to_update[j]->done = 1;
					} else {
						criteria_to_update[j] ->done = 0;
					}
				}
			}
		} else {
			advancements[i]->done = 0;
		}
	}

	cJSON_Delete(data);
	return;

error:
	printf("[ERROR] Couldn't update the advancement data.\n");
	cJSON_Delete(data);
}