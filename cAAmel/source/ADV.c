#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adv.h"

ADV_advancement* ADV_new_advancement(char* name, char* display_name, char* icon, char* root_name, int done) {
	ADV_advancement* advancement = malloc(sizeof(ADV_advancement));
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

	//strcpy_s(advancement->name, name, sizeof(advancement->name));
	//strcpy_s(advancement->display_name, display_name, sizeof(advancement->display_name));
	//strcpy_s(advancement->icon, icon, sizeof(advancement->icon));
	advancement->name = name;
	advancement->display_name = display_name;
	advancement->icon = icon;
	advancement->root_name = root_name;
	advancement->done = 0;

	return advancement;
}

void ADV_delete_advancement(ADV_advancement* advancement) {
	if (advancement != NULL) {
		free(advancement->name);
		free(advancement->display_name);
		free(advancement->icon);
		free(advancement->root_name);
		free(advancement);
	}
}

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

ADV_advancement** ADV_object_from_template(cJSON* template, int n) {
	cJSON* entry = template->child;
	
	ADV_advancement** advancements = malloc(sizeof(ADV_advancement)*n); 

	for (int i = 0; entry; ++i) {
		cJSON* done = cJSON_GetObjectItemCaseSensitive(entry, "done");
		cJSON* name = cJSON_GetObjectItemCaseSensitive(entry, "name");
		cJSON* display_name = cJSON_GetObjectItemCaseSensitive(entry, "displayName");
		cJSON* icon = cJSON_GetObjectItemCaseSensitive(entry, "icon");
		cJSON* root_name = cJSON_GetObjectItemCaseSensitive(entry, "rootName");
		
		if (!done || !name || !display_name || !icon || !root_name || !root_name->valuestring || !name->valuestring || !display_name->valuestring || !icon->valuestring) {
			printf("[TEMPLATE ERROR]: Inavlid template file.");
			
			for (int j = 0; j < i; ++j) {
				ADV_delete_advancement(advancements[j]);
			}

			free(advancements);

			return NULL;
		}
		
		int done_ = 0;

		if (done && cJSON_IsTrue(done)) {
			done_ = 1;
		}

		advancements[i] = ADV_new_advancement(
			_strdup(name->valuestring), 
			_strdup(display_name->valuestring),
			_strdup(icon->valuestring),
			_strdup(root_name->valuestring),
			done_
		);

		entry = entry->next;
	}

	return advancements;
}

void ADV_update_advancements(ADV_advancement** advancements, int n, char* path) {
	cJSON* data = ADV_get_json(path);
	if (data == NULL) {
		goto error;
	}

	for (int i = 0; i < n; ++i) {
		char* root_name = advancements[i]->root_name;

		cJSON* entry = cJSON_GetObjectItemCaseSensitive(data, root_name);
		if (entry) {
			cJSON* done = cJSON_GetObjectItemCaseSensitive(entry, "done");

			if (!done) {
				goto error;
			}

			if (cJSON_IsTrue(done)) {
				advancements[i]->done = 1;
			}
		} else {
			advancements[i]->done = 0;
		}
	}

	cJSON_Delete(data);
	return;

error:
	cJSON_Delete(data);
}