#include <string.h>
#include <stdio.h>

int
bitflag_to_idx(int val) {
	int i;

	for (i = 0; val; val >>= 1, i++)
		if (val & 1)
			return i;

	return -1;
}

void
tokenize_path(char **tokens, char *path) {
	char dup_path[strlen(path)+1];
	char *token;
	int i;

	i = 0;
	strcpy(dup_path, path);

	token = strtok(dup_path, "/");
	while (token != NULL) {
		tokens[i++] = token;
		token = strtok(NULL, "/");
	}
}

char *
get_entry_name(char *path) {
	char *marker;
	char *entry_name;

	marker = path;
	entry_name = NULL;

	if (strcmp(marker, "/") == 0)
		return marker;

	for (; *marker != '\0'; marker++) {
		if (*marker == '/')
			entry_name = marker+1;
	}

	printf("path is: %s\n", path);

	return entry_name;
}
